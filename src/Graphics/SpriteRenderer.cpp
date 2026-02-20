#include "SpriteRenderer.hpp"

#include "Core/Logger.hpp"
#include "Vertex.hpp"

namespace Graphics {

SpriteRenderer::SpriteRenderer(DX11Device* device)
  : m_device(device)
{
  if (!m_device || !(m_device->getDevice())) {
    LOG_ERROR("Null DX11Device passed to SpriteRenderer.");
    throw std::invalid_argument("DX11Device is null.");
  }
}

void SpriteRenderer::initialize()
{
  LOG_INFO("Initializing SpriteRenderer Pipeline...");

  initShaders();
  initBuffers();
  initStates();
  m_instances.reserve(m_maxInstances);

  LOG_INFO("SpriteRenderer Pipeline initialized successfully.");
}

void SpriteRenderer::updateProjectionMatrix(float windowWidth, float windowHeight)
{
  // 预计算 2D 正交矩阵投影
  DirectX::XMMATRIX orthoMatrix =
    DirectX::XMMatrixOrthographicOffCenterLH(0.0f, windowWidth, windowHeight, 0.0f, 0.0f, 1.0f);
  // CPU(C++) 默认是行主序 (Row-Major) 矩阵, GPU(HLSL) 默认是列主序 (Column-Major) 矩阵
  // 在传给 GPU 之前, 必须进行一次转置
  DirectX::XMStoreFloat4x4(&m_projectionMatrix, DirectX::XMMatrixTranspose(orthoMatrix));
}

void SpriteRenderer::begin()
{
  // 开始新的一帧, 清空实例数据和当前绑定的贴图
  m_instances.clear();
  m_currentTexture = nullptr;

  auto context = m_device->getContext();

  // 将状态绑定到渲染管线
  m_inputLayout->Bind(context);
  m_vertexShader->Bind(context);
  m_pixelShader->Bind(context);
  // 绑定光栅化状态
  context->RSSetState(m_rasterizerState.Get());
  // 把采样器绑定到像素着色器 (PS) 的第 0 号槽位
  context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
  // 绑定混合状态, nullptr 表示不使用混合因子常量, 0xffffffff 表示所有多重采样遮罩全开
  context->OMSetBlendState(m_blendState.Get(), nullptr, 0xffffffff);
  // 拓扑结构: 告诉 GPU 传来的是一系列三角形
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // 将投影矩阵更新到常量缓冲区
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  HRESULT hr = context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
  LOG_DX11_CHECK(hr, "Failed to map Constant Buffer!");
  TransformCBuffer* dataPtr = static_cast<TransformCBuffer*>(mappedResource.pData);
  dataPtr->projection = m_projectionMatrix;
  context->Unmap(m_constantBuffer.Get(), 0);

  // 绑定常量缓冲区到顶点着色器
  context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
}

void SpriteRenderer::end()
{
  flush();
}

void SpriteRenderer::drawSprite(Texture* texture, float x, float y, float angle, float scaleX, float scaleY)
{
  if (!texture) {
    return;
  }

  // 核心批处理逻辑: 如果我们换了一张贴图, 或者 m_instances 已经塞满了, 立刻把现有的货物发走 (flush)，然后再装新货
  if (texture != m_currentTexture || m_instances.size() >= m_maxInstances) {
    flush();
    m_currentTexture = texture;
  }

  // 悄悄把数据塞进 vector, 先不呼叫 GPU
  InstanceData data;
  data.position = { x, y };
  data.scale = { scaleX, scaleY };
  data.rotation = angle;
  data.color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 默认白色 (原图颜色)

  m_instances.push_back(data);
}

void SpriteRenderer::initShaders()
{
  // 检测文件是否存在
  std::filesystem::path shaderPath = std::filesystem::current_path() / "assets/shaders/Sprite.hlsl";
  if (!std::filesystem::exists(shaderPath)) {
    LOG_ERROR("Shader file missing: " + shaderPath.string());
    throw std::runtime_error("Shader file missing.");
  }

  // 编译着色器 VS 与 PS
  auto vsBytecode = ShaderCompiler::compile(shaderPath.string(), "VSMain", "vs_5_0");
  m_vertexShader = std::make_unique<VertexShader>(m_device->getDevice(), vsBytecode.Get());
  auto psBytecode = ShaderCompiler::compile(shaderPath.string(), "PSMain", "ps_5_0");
  m_pixelShader = std::make_unique<PixelShader>(m_device->getDevice(), psBytecode.Get());

  // 创建输入布局 (Input Layout) (必须与 hlsl 中的布局完全匹配)
  std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDesc = {
    // 槽位 0：顶点缓冲区数据 (PER_VERTEX_DATA)
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

    // 槽位 1：实例缓冲区数据 (PER_INSTANCE_DATA)
    // 最后的 `1`, 它代表 InstanceDataStepRate, 意思是“画完 1 个完整的实例（包含 4 个顶点）后, 才前进读取下一个实例数据”
    { "INST_POS", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "INST_SCALE", 0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "INST_ROT", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "INST_COLOR",
      0,
      DXGI_FORMAT_R32G32B32A32_FLOAT,
      1,
      D3D11_APPEND_ALIGNED_ELEMENT,
      D3D11_INPUT_PER_INSTANCE_DATA,
      1 }
  };
  m_inputLayout = std::make_unique<InputLayout>(m_device->getDevice(), layoutDesc, vsBytecode.Get());
}

void SpriteRenderer::initBuffers()
{
  // 创建顶点缓冲区
  // 目前没有使用矩阵变换, 直接使用 GPU 的归一化设备坐标 (NDC)
  // NDC 坐标系中, 屏幕中心是 (0,0), 左下角是 (-1,-1), 右上角是 (1,1)
  Vertex vertices[] = {
    Vertex(-0.5f, -0.5f, 0.0f, 0.0f, 0.0f), // 左下角 (索引 0)
    Vertex(-0.5f, 0.5f, 0.0f, 0.0f, 1.0f),  // 左上角 (索引 1)
    Vertex(0.5f, 0.5f, 0.0f, 1.0f, 1.0f),   // 右上角 (索引 2)
    Vertex(0.5f, -0.5f, 0.0f, 1.0f, 0.0f)   // 右下角 (索引 3)
  };

  D3D11_BUFFER_DESC vbd{};
  vbd.Usage = D3D11_USAGE_IMMUTABLE; // 声明这些顶点永远不会变, 显卡会把它放在最快的内存区
  vbd.ByteWidth = sizeof(vertices);
  vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 告诉显卡这个是顶点缓冲区
  vbd.CPUAccessFlags = 0;

  D3D11_SUBRESOURCE_DATA vInitData{};
  vInitData.pSysMem = vertices; // 指向 C++ 内存中的数据

  HRESULT hr = m_device->getDevice()->CreateBuffer(&vbd, &vInitData, m_vertexBuffer.GetAddressOf());
  LOG_DX11_CHECK(hr, "Failed to create Vertex Buffer.");

  // 创建索引缓冲区
  // 定义如何连接上述 4 个顶点来组成 2 个三角形 (顺时针)
  unsigned int indices[] = {
    0, 1, 2, // 第一个三角形: 左下, 左上, 右上
    0, 2, 3  // 第二个三角形: 左下, 右上, 右下
  };

  D3D11_BUFFER_DESC ibd{};
  ibd.Usage = D3D11_USAGE_IMMUTABLE;
  ibd.ByteWidth = sizeof(indices);
  ibd.BindFlags = D3D11_BIND_INDEX_BUFFER; // 告诉显卡这是个索引缓冲区
  ibd.CPUAccessFlags = 0;

  D3D11_SUBRESOURCE_DATA iInitData{};
  iInitData.pSysMem = indices;

  hr = m_device->getDevice()->CreateBuffer(&ibd, &iInitData, m_indexBuffer.GetAddressOf());
  LOG_DX11_CHECK(hr, "Failed to create Index Buffer.");

  // 创建常量缓冲区
  D3D11_BUFFER_DESC cbd{};
  cbd.Usage = D3D11_USAGE_DYNAMIC;
  cbd.ByteWidth = sizeof(TransformCBuffer);
  cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

  hr = m_device->getDevice()->CreateBuffer(&cbd, nullptr, m_constantBuffer.GetAddressOf());
  LOG_DX11_CHECK(hr, "Failed to create constant buffer.");

  // 创建实例缓冲区 (Instance Buffer)
  D3D11_BUFFER_DESC instDesc{};
  instDesc.Usage = D3D11_USAGE_DYNAMIC; // 动态, CPU 每一帧都要把几千颗子弹塞进去
  instDesc.ByteWidth = sizeof(InstanceData) * m_maxInstances;
  instDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;    // 实例缓冲区本质上也是一种顶点缓冲区
  instDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // 允许 CPU 写入

  hr = m_device->getDevice()->CreateBuffer(&instDesc, nullptr, m_instanceBuffer.GetAddressOf());
  LOG_DX11_CHECK(hr, "Failed to create Instance Buffer.");
}

void SpriteRenderer::initStates()
{
  // 创建光栅化器状态
  D3D11_RASTERIZER_DESC rd{};
  rd.FillMode = D3D11_FILL_SOLID;   // 实心填充
  rd.CullMode = D3D11_CULL_NONE;    // 关闭背面剔除
  rd.FrontCounterClockwise = FALSE; // 默认正面是顺时针
  rd.DepthClipEnable = TRUE;

  HRESULT hr = m_device->getDevice()->CreateRasterizerState(&rd, m_rasterizerState.GetAddressOf());
  LOG_DX11_CHECK(hr, "Failed to create Rasterizer State.");

  // 创建采样器状态
  D3D11_SAMPLER_DESC sampDesc{};
  sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // 缩放使用线性平滑插值

  sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

  sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  sampDesc.MinLOD = 0;
  sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

  hr = m_device->getDevice()->CreateSamplerState(&sampDesc, m_samplerState.GetAddressOf());
  LOG_DX11_CHECK(hr, "Failed to create Sampler State.");

  // 创建 Alpha 混合状态
  D3D11_BLEND_DESC blendDesc{};
  blendDesc.RenderTarget[0].BlendEnable = TRUE; // 开启混合

  // 混合公式: 最终颜色 = (贴图颜色 * 贴图Alpha) + (背景颜色 * (1 - 贴图Alpha))
  blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
  blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

  // Alpha 通道的混合逻辑 (通常直接覆盖或相加)
  blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
  blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
  blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

  // 允许写入所有颜色通道
  blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

  hr = m_device->getDevice()->CreateBlendState(&blendDesc, m_blendState.GetAddressOf());
  LOG_DX11_CHECK(hr, "Failed to create Blend State.");
}

void SpriteRenderer::flush()
{
  if (m_instances.empty() || !m_currentTexture) {
    return;
  }

  auto context = m_device->getContext();

  // 把 vector 里的数据通过内存拷贝到 Instance Buffer (显存)
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  HRESULT hr = context->Map(m_instanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
  LOG_DX11_CHECK(hr, "Failed to map Instance Buffer!");
  memcpy(mappedResource.pData, m_instances.data(), sizeof(InstanceData) * m_instances.size());

  context->Unmap(m_instanceBuffer.Get(), 0);

  // 同时绑定两个顶点缓冲区
  ID3D11Buffer* vbs[] = { m_vertexBuffer.Get(), m_instanceBuffer.Get() };
  UINT strides[] = { sizeof(Vertex), sizeof(InstanceData) };
  UINT offsets[] = { 0, 0 };
  // 槽位0是几何顶点, 槽位1是实例数据. (在 InputLayout 中与 GPU 约定)
  context->IASetVertexBuffers(0, 2, vbs, strides, offsets);

  context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

  // 绑定当前批次的贴图
  ID3D11ShaderResourceView* srvs[] = { m_currentTexture->getSRV() };
  context->PSSetShaderResources(0, 1, srvs);

  // 参数: 每个实例的索引数(6), 实例总数, 起始索引(0), 顶点起始偏移(0), 实例起始偏移(0)
  context->DrawIndexedInstanced(6, static_cast<UINT>(m_instances.size()), 0, 0, 0);

  // 货物送达, 清空车厢, 准备装下一批货物
  m_instances.clear();
}

} // namespace Graphics
