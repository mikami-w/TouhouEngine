#include "SpriteRenderer.hpp"

#include <cstring>
#include <filesystem>

#include "Core/Logger.hpp"
#include "GFXDevice.hpp"
#include "ShaderLoader.hpp"
#include "Texture.hpp"

namespace Graphics {

SpriteRenderer::SpriteRenderer(GFXDevice* gfx)
  : m_gfx(gfx)
{
  if (!m_gfx || !m_gfx->getDevice()) {
    LOG_ERROR("Null GFXDevice passed to SpriteRenderer.");
    throw std::invalid_argument("GFXDevice is null.");
  }
}

void SpriteRenderer::initialize(SDL_Window* window)
{
  LOG_INFO("Initializing SpriteRenderer Pipeline...");

  initShaders(window);
  initPipeline(window);
  initBuffers();
  initSampler();
  m_instances.reserve(m_maxInstances);

  LOG_INFO("SpriteRenderer Pipeline initialized successfully.");
}

void SpriteRenderer::setView(float screenX, float screenY, float screenW, float screenH, float logicalW, float logicalH)
{
  flush(); // 切换视口前先把当前积累的实例发走

  m_viewX = screenX;
  m_viewY = screenY;
  m_viewW = screenW;
  m_viewH = screenH;

  // 内部逻辑坐标系
  float lw = (logicalW < 0.0f) ? screenW : logicalW;
  float lh = (logicalH < 0.0f) ? screenH : logicalH;

  // 构建正交投影矩阵 (行主序, 与 HLSL mul(vector, matrix) 匹配)
  float r = lw, l = 0.0f, t = 0.0f, b = lh, n = 0.0f, f = 1.0f;
  std::memset(m_projectionMatrix, 0, sizeof(m_projectionMatrix));
  m_projectionMatrix[0] = 2.0f / (r - l);
  m_projectionMatrix[5] = 2.0f / (t - b);
  m_projectionMatrix[10] = 1.0f / (f - n);
  m_projectionMatrix[12] = (l + r) / (l - r);
  m_projectionMatrix[13] = (t + b) / (b - t);
  m_projectionMatrix[14] = n / (n - f);
  m_projectionMatrix[15] = 1.0f;
}

void SpriteRenderer::beginFrame(SDL_Window* window)
{
  // 1. 获取 command buffer
  m_cmd = SDL_AcquireGPUCommandBuffer(m_gfx->getDevice());
  if (!m_cmd) {
    LOG_ERROR(std::format("SDL_AcquireGPUCommandBuffer failed: {}", SDL_GetError()));
    return;
  }

  // 2. 获取 swapchain texture
  auto swapchain = m_gfx->acquireSwapchain(m_cmd, window);
  m_swapchainTexture = swapchain.texture;
  m_swapchainWidth = swapchain.width;
  m_swapchainHeight = swapchain.height;

  if (!m_swapchainTexture) {
    // 窗口最小化等, 无法渲染
    SDL_SubmitGPUCommandBuffer(m_cmd);
    m_cmd = nullptr;
    return;
  }

  // 3. 开始 render pass (清屏黑色)
  SDL_GPUColorTargetInfo colorTarget{};
  colorTarget.texture = m_swapchainTexture;
  colorTarget.clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
  colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
  colorTarget.store_op = SDL_GPU_STOREOP_STORE;

  m_renderPass = SDL_BeginGPURenderPass(m_cmd, &colorTarget, 1, nullptr);
}

void SpriteRenderer::begin()
{
  m_instances.clear();
  m_currentTexture = nullptr;

  if (!m_renderPass || !m_cmd) {
    return;
  }

  // 绑定管线 (PSO: 所有状态一次绑定)
  SDL_BindGPUGraphicsPipeline(m_renderPass, m_pipeline.get());

  // 设置视口
  SDL_GPUViewport viewport{};
  viewport.x = m_viewX;
  viewport.y = m_viewY;
  viewport.w = m_viewW;
  viewport.h = m_viewH;
  viewport.min_depth = 0.0f;
  viewport.max_depth = 1.0f;
  SDL_SetGPUViewport(m_renderPass, &viewport);

  // 推送投影矩阵 uniform
  SDL_PushGPUVertexUniformData(m_cmd, 0, m_projectionMatrix, sizeof(m_projectionMatrix));
}

void SpriteRenderer::end()
{
  flush();
}

void SpriteRenderer::endFrame()
{
  if (m_renderPass) {
    SDL_EndGPURenderPass(m_renderPass);
    m_renderPass = nullptr;
  }
  if (m_cmd) {
    SDL_SubmitGPUCommandBuffer(m_cmd);
    m_cmd = nullptr;
  }
}

void SpriteRenderer::drawSprite(Texture* texture,
                                Core::Vector2 pos,
                                float angle,
                                Core::Vector2 scale,
                                Core::Vector4 uvRect,
                                Core::Vector4 color)
{
  if (!texture) {
    return;
  }

  if (texture != m_currentTexture || m_instances.size() >= m_maxInstances) {
    flush();
    m_currentTexture = texture;
  }

  m_instances.emplace_back(pos, scale, angle, uvRect, color);
}

void SpriteRenderer::drawText(Texture* texture,
                              std::string const& text,
                              Core::Vector2 start,
                              float scale,
                              Core::Vector4 color)
{
  if (!texture) {
    return;
  }

  constexpr int cols = 16;
  constexpr float uvStep = 1.0f / cols;

  float const quadSize = 32.0f * scale;
  float const advanceX = 16.0f * scale;

  float cursorX = start.x;

  for (char c : text) {
    if (c >= ' ' && c <= '~') {
      int index = c - ' ';
      int row = index / cols;
      int col = index % cols;

      Core::Vector4 uvRect = {
        col * uvStep,
        row * uvStep,
        uvStep,
        uvStep
      };

      drawSprite(texture, { cursorX, start.y }, 0.0f, { quadSize, quadSize }, uvRect, color);
    }

    cursorX += advanceX;
  }
}

void SpriteRenderer::initShaders(SDL_Window* /*window*/)
{
  auto* device = m_gfx->getDevice();
  std::string shaderPath = (std::filesystem::current_path() / "assets/shaders/compiled").string();

  // 顶点着色器: 1 个 uniform buffer (投影矩阵)
  ShaderResourceInfo vsResources{};
  vsResources.numUniformBuffers = 1;

  m_vertexShader = ShaderLoader::load(device, shaderPath, "Sprite.vert", SDL_GPU_SHADERSTAGE_VERTEX, vsResources);

  // 片段着色器: 1 个 sampler (纹理)
  ShaderResourceInfo fsResources{};
  fsResources.numSamplers = 1;

  m_fragmentShader = ShaderLoader::load(device, shaderPath, "Sprite.frag", SDL_GPU_SHADERSTAGE_FRAGMENT, fsResources);
}

void SpriteRenderer::initPipeline(SDL_Window* /*window*/)
{
  auto* device = m_gfx->getDevice();
  SDL_GPUTextureFormat swapchainFormat = m_gfx->getSwapchainFormat();

  // --- 顶点输入布局 ---
  SDL_GPUVertexBufferDescription bufDescs[2]{};

  // 槽位 0: 顶点数据 (PER_VERTEX)
  bufDescs[0].slot = 0;
  bufDescs[0].pitch = sizeof(Vertex);
  bufDescs[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
  bufDescs[0].instance_step_rate = 0;

  // 槽位 1: 实例数据 (PER_INSTANCE)
  bufDescs[1].slot = 1;
  bufDescs[1].pitch = sizeof(InstanceData);
  bufDescs[1].input_rate = SDL_GPU_VERTEXINPUTRATE_INSTANCE;
  bufDescs[1].instance_step_rate = 0;

  SDL_GPUVertexAttribute attrs[7]{};

  // 槽位 0 的属性 (每顶点)
  attrs[0].location = 0;
  attrs[0].buffer_slot = 0;
  attrs[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
  attrs[0].offset = offsetof(Vertex, position);

  attrs[1].location = 1;
  attrs[1].buffer_slot = 0;
  attrs[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
  attrs[1].offset = offsetof(Vertex, texCoord);

  // 槽位 1 的属性 (每实例)
  attrs[2].location = 2;
  attrs[2].buffer_slot = 1;
  attrs[2].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
  attrs[2].offset = offsetof(InstanceData, position);

  attrs[3].location = 3;
  attrs[3].buffer_slot = 1;
  attrs[3].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
  attrs[3].offset = offsetof(InstanceData, scale);

  attrs[4].location = 4;
  attrs[4].buffer_slot = 1;
  attrs[4].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT;
  attrs[4].offset = offsetof(InstanceData, rotation);

  attrs[5].location = 5;
  attrs[5].buffer_slot = 1;
  attrs[5].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
  attrs[5].offset = offsetof(InstanceData, uvRect);

  attrs[6].location = 6;
  attrs[6].buffer_slot = 1;
  attrs[6].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
  attrs[6].offset = offsetof(InstanceData, color);

  SDL_GPUVertexInputState vertexInput{};
  vertexInput.vertex_buffer_descriptions = bufDescs;
  vertexInput.num_vertex_buffers = 2;
  vertexInput.vertex_attributes = attrs;
  vertexInput.num_vertex_attributes = 7;

  // --- 光栅化状态 ---
  SDL_GPURasterizerState rasterizer{};
  rasterizer.fill_mode = SDL_GPU_FILLMODE_FILL;
  rasterizer.cull_mode = SDL_GPU_CULLMODE_NONE;
  rasterizer.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
  rasterizer.enable_depth_clip = false;

  // --- 多重采样 ---
  SDL_GPUMultisampleState multisample{};
  multisample.sample_count = SDL_GPU_SAMPLECOUNT_1;
  multisample.enable_alpha_to_coverage = false;

  // --- 深度模板 (2D 不需要) ---
  SDL_GPUDepthStencilState depthStencil{};
  depthStencil.enable_depth_test = false;
  depthStencil.enable_depth_write = false;
  depthStencil.enable_stencil_test = false;

  // --- 混合状态 (Alpha 混合) ---
  SDL_GPUColorTargetBlendState blendState{};
  blendState.enable_blend = true;
  blendState.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
  blendState.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
  blendState.color_blend_op = SDL_GPU_BLENDOP_ADD;
  blendState.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
  blendState.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
  blendState.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
  blendState.enable_color_write_mask = false;

  SDL_GPUColorTargetDescription colorTargetDesc{};
  colorTargetDesc.format = swapchainFormat;
  colorTargetDesc.blend_state = blendState;

  SDL_GPUGraphicsPipelineTargetInfo targetInfo{};
  targetInfo.color_target_descriptions = &colorTargetDesc;
  targetInfo.num_color_targets = 1;
  targetInfo.has_depth_stencil_target = false;

  // --- 创建 PSO ---
  SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.vertex_shader = m_vertexShader.get();
  pipelineInfo.fragment_shader = m_fragmentShader.get();
  pipelineInfo.vertex_input_state = vertexInput;
  pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
  pipelineInfo.rasterizer_state = rasterizer;
  pipelineInfo.multisample_state = multisample;
  pipelineInfo.depth_stencil_state = depthStencil;
  pipelineInfo.target_info = targetInfo;

  m_pipeline = makePipeline(device, pipelineInfo);
  if (!m_pipeline) {
    LOG_ERROR(std::format("Failed to create graphics pipeline: {}", SDL_GetError()));
    throw std::runtime_error("Failed to create graphics pipeline.");
  }

  LOG_INFO("Sprite PSO created successfully.");
}

void SpriteRenderer::initBuffers()
{
  auto* device = m_gfx->getDevice();

  // 顶点缓冲区 (4 个顶点构成一个 quad)
  Vertex vertices[] = {
    Vertex{ -0.5f, -0.5f, 0.0f, 0.0f, 0.0f },
    Vertex{ -0.5f,  0.5f, 0.0f, 0.0f, 1.0f },
    Vertex{  0.5f,  0.5f, 0.0f, 1.0f, 1.0f },
    Vertex{  0.5f, -0.5f, 0.0f, 1.0f, 0.0f },
  };

  SDL_GPUBufferCreateInfo vboInfo{};
  vboInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
  vboInfo.size = sizeof(vertices);
  m_vertexBuffer = makeBuffer(device, vboInfo);

  // 顶点数据需要通过 transfer buffer 上传
  SDL_GPUTransferBufferCreateInfo vboTransferInfo{};
  vboTransferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
  vboTransferInfo.size = sizeof(vertices);
  TransferBufPtr vboTransfer = makeTransferBuffer(device, vboTransferInfo);

  auto* vboMapped = static_cast<Vertex*>(SDL_MapGPUTransferBuffer(device, vboTransfer.get(), false));
  std::memcpy(vboMapped, vertices, sizeof(vertices));
  SDL_UnmapGPUTransferBuffer(device, vboTransfer.get());

  SDL_GPUCommandBuffer* uploadCmd = SDL_AcquireGPUCommandBuffer(device);
  SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmd);

  SDL_GPUTransferBufferLocation srcLoc{};
  srcLoc.transfer_buffer = vboTransfer.get();
  srcLoc.offset = 0;

  SDL_GPUBufferRegion dstRegion{};
  dstRegion.buffer = m_vertexBuffer.get();
  dstRegion.offset = 0;
  dstRegion.size = sizeof(vertices);

  SDL_UploadToGPUBuffer(copyPass, &srcLoc, &dstRegion, false);
  SDL_EndGPUCopyPass(copyPass);
  SDL_SubmitGPUCommandBuffer(uploadCmd);

  // 索引缓冲区 (2 个三角形)
  Uint32 indices[] = { 0, 1, 2, 0, 2, 3 };

  SDL_GPUBufferCreateInfo iboInfo{};
  iboInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;
  iboInfo.size = sizeof(indices);
  m_indexBuffer = makeBuffer(device, iboInfo);

  SDL_GPUTransferBufferCreateInfo iboTransferInfo{};
  iboTransferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
  iboTransferInfo.size = sizeof(indices);
  TransferBufPtr iboTransfer = makeTransferBuffer(device, iboTransferInfo);

  auto* iboMapped = static_cast<Uint32*>(SDL_MapGPUTransferBuffer(device, iboTransfer.get(), false));
  std::memcpy(iboMapped, indices, sizeof(indices));
  SDL_UnmapGPUTransferBuffer(device, iboTransfer.get());

  SDL_GPUCommandBuffer* uploadCmd2 = SDL_AcquireGPUCommandBuffer(device);
  SDL_GPUCopyPass* copyPass2 = SDL_BeginGPUCopyPass(uploadCmd2);

  SDL_GPUTransferBufferLocation srcLoc2{};
  srcLoc2.transfer_buffer = iboTransfer.get();
  srcLoc2.offset = 0;

  SDL_GPUBufferRegion dstRegion2{};
  dstRegion2.buffer = m_indexBuffer.get();
  dstRegion2.offset = 0;
  dstRegion2.size = sizeof(indices);

  SDL_UploadToGPUBuffer(copyPass2, &srcLoc2, &dstRegion2, false);
  SDL_EndGPUCopyPass(copyPass2);
  SDL_SubmitGPUCommandBuffer(uploadCmd2);

  // 实例缓冲区 (动态, 每帧更新)
  SDL_GPUBufferCreateInfo instBufInfo{};
  instBufInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
  instBufInfo.size = static_cast<Uint32>(sizeof(InstanceData) * m_maxInstances);
  m_instanceBuffer = makeBuffer(device, instBufInfo);

  // 实例 Transfer Buffer (用于每帧上传)
  SDL_GPUTransferBufferCreateInfo instTransferInfo{};
  instTransferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
  instTransferInfo.size = static_cast<Uint32>(sizeof(InstanceData) * m_maxInstances);
  m_instanceTransferBuffer = makeTransferBuffer(device, instTransferInfo);
}

void SpriteRenderer::initSampler()
{
  // 采样器在 TextureManager 中已经创建并共享
  // SpriteRenderer 不直接管理采样器, 纹理绑定时使用 Texture 自带的 sampler
}

void SpriteRenderer::flush()
{
  if (m_instances.empty() || !m_currentTexture || !m_cmd) {
    return;
  }

  auto* device = m_gfx->getDevice();

  // SDL3 GPU 不允许在 render pass 进行中开始 copy pass
  // 必须先结束当前 render pass, 完成 copy, 再重新开始 render pass
  if (m_renderPass) {
    SDL_EndGPURenderPass(m_renderPass);
    m_renderPass = nullptr;
  }

  // 上传实例数据到 transfer buffer
  auto* mapped = static_cast<Uint8*>(
    SDL_MapGPUTransferBuffer(device, m_instanceTransferBuffer.get(), true));
  if (!mapped) {
    LOG_ERROR("Failed to map instance transfer buffer.");
    m_instances.clear();
    return;
  }
  std::memcpy(mapped, m_instances.data(), sizeof(InstanceData) * m_instances.size());
  SDL_UnmapGPUTransferBuffer(device, m_instanceTransferBuffer.get());

  // Copy Pass: 从 transfer buffer 上传到 instance buffer
  SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(m_cmd);

  SDL_GPUTransferBufferLocation transferSrc{};
  transferSrc.transfer_buffer = m_instanceTransferBuffer.get();
  transferSrc.offset = 0;

  SDL_GPUBufferRegion bufferDst{};
  bufferDst.buffer = m_instanceBuffer.get();
  bufferDst.offset = 0;
  bufferDst.size = static_cast<Uint32>(sizeof(InstanceData) * m_instances.size());

  SDL_UploadToGPUBuffer(copyPass, &transferSrc, &bufferDst, true);
  SDL_EndGPUCopyPass(copyPass);

  // 重新开始 render pass (LOAD 保留已有内容, 不清除)
  if (m_swapchainTexture) {
    SDL_GPUColorTargetInfo colorTarget{};
    colorTarget.texture = m_swapchainTexture;
    colorTarget.load_op = SDL_GPU_LOADOP_LOAD;
    colorTarget.store_op = SDL_GPU_STOREOP_STORE;
    m_renderPass = SDL_BeginGPURenderPass(m_cmd, &colorTarget, 1, nullptr);
  }

  if (!m_renderPass) {
    m_instances.clear();
    return;
  }

  // 重新绑定管线、视口和 uniform
  SDL_BindGPUGraphicsPipeline(m_renderPass, m_pipeline.get());

  SDL_GPUViewport viewport{};
  viewport.x = m_viewX;
  viewport.y = m_viewY;
  viewport.w = m_viewW;
  viewport.h = m_viewH;
  viewport.min_depth = 0.0f;
  viewport.max_depth = 1.0f;
  SDL_SetGPUViewport(m_renderPass, &viewport);

  SDL_PushGPUVertexUniformData(m_cmd, 0, m_projectionMatrix, sizeof(m_projectionMatrix));

  // 绑定顶点缓冲区 (槽位 0: 几何顶点, 槽位 1: 实例数据)
  SDL_GPUBufferBinding vboBindings[2]{};
  vboBindings[0].buffer = m_vertexBuffer.get();
  vboBindings[0].offset = 0;
  vboBindings[1].buffer = m_instanceBuffer.get();
  vboBindings[1].offset = 0;
  SDL_BindGPUVertexBuffers(m_renderPass, 0, vboBindings, 2);

  // 绑定索引缓冲区
  SDL_GPUBufferBinding iboBinding{};
  iboBinding.buffer = m_indexBuffer.get();
  iboBinding.offset = 0;
  SDL_BindGPUIndexBuffer(m_renderPass, &iboBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

  // 绑定纹理 + 采样器到 fragment 阶段
  SDL_GPUTextureSamplerBinding texBinding = m_currentTexture->getBinding();
  SDL_BindGPUFragmentSamplers(m_renderPass, 0, &texBinding, 1);

  // 绘制: 每实例 6 个索引 (2 个三角形), N 个实例
  SDL_DrawGPUIndexedPrimitives(m_renderPass, 6, static_cast<Uint32>(m_instances.size()), 0, 0, 0);

  m_instances.clear();
}

} // namespace Graphics
