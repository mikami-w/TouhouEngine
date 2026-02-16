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
  initRasterizerState();

  LOG_INFO("SpriteRenderer Pipeline initialized successfully.");
}

void SpriteRenderer::updateProjectionMatrix(float windowWidth, float windowHeight)
{
  // 预计算 2D 正交矩阵投影
  DirectX::XMMATRIX orthoMatrix =
    DirectX::XMMatrixOrthographicOffCenterLH(0.0f, windowWidth, windowHeight, 0.0f, 0.0f, 1.0f);
  // CPU(C++) 默认是行主序 (Row-Major) 矩阵, GPU(HLSL) 默认是列主序 (Column-Major) 矩阵
  // 在传给 GPU 之前，必须进行一次转置
  DirectX::XMStoreFloat4x4(&m_projectionMatrix, DirectX::XMMatrixTranspose(orthoMatrix));
}

void SpriteRenderer::begin()
{
  auto context = m_device->getContext();

  // 将状态绑定到渲染管线
  m_inputLayout->Bind(context);
  m_vertexShader->Bind(context);
  m_pixelShader->Bind(context);

  // 绑定光栅化状态
  context->RSSetState(m_rasterizerState.Get());

  // 拓扑结构: 告诉 GPU 传来的是一系列三角形
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void SpriteRenderer::end()
{
  // 目前暂不需要清理状态, 留作扩展口
}

void SpriteRenderer::drawTestQuad(float x, float y, float angle, float scaleX, float scaleY)
{
  auto context = m_device->getContext();

  // 计算当前方块的世界矩阵(缩放->旋转->平移)
  DirectX::XMMATRIX scaling = DirectX::XMMatrixScaling(scaleX, scaleY, 1.0f);
  DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationZ(angle);
  DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(x, y, 0.0f);
  DirectX::XMMATRIX worldMatrix = scaling * rotation * translation;

  // 将数据推送到显存 (Map / Unmap 模式)
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  // Map: 锁定一块区域让 CPU 写入
  HRESULT hr = m_device->getContext()->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
  if (SUCCEEDED(hr)) {
    TransformCBuffer* dataPtr = (TransformCBuffer*)mappedResource.pData;

    dataPtr->projection = m_projectionMatrix;
    DirectX::XMStoreFloat4x4(&dataPtr->world, DirectX::XMMatrixTranspose(worldMatrix));

    // 解除锁定
    context->Unmap(m_constantBuffer.Get(), 0);
  }
  // TODO: 失败处理

  // 将常量缓冲区绑定到管线的 VS 的 0 号槽位
  context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

  // 绑定顶点缓冲区
  UINT stride = sizeof(Vertex); // 告诉显卡每个顶点跨度多大
  UINT offset = 0;
  context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

  // 绑定索引缓冲区
  context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

  // Draw Call
  context->DrawIndexed(6, 0, 0);
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
    // 语义名 | 语义索引 | 数据格式 (RGB32F 即 3个float) | 输入槽 | 字节偏移 | 数据类型 | 实例步进
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    // 纹理坐标紧跟在 position 后面，position 占了 12 字节，所以偏移量为 12 (D3D11_APPEND_ALIGNED_ELEMENT 自动计算)
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
  };
  m_inputLayout = std::make_unique<InputLayout>(m_device->getDevice(), layoutDesc, vsBytecode.Get());
}

void SpriteRenderer::initBuffers()
{
  // 创建顶点缓冲区
  // 目前没有使用矩阵变换, 直接使用 GPU 的归一化设备坐标 (NDC)
  // NDC 坐标系中，屏幕中心是 (0,0)，左下角是 (-1,-1)，右上角是 (1,1)
  Vertex vertices[] = {
    Vertex(-0.5f, -0.5f, 0.0f, 0.0f, 1.0f), // 左下角 (索引 0)
    Vertex(-0.5f, 0.5f, 0.0f, 0.0f, 0.0f),  // 左上角 (索引 1)
    Vertex(0.5f, 0.5f, 0.0f, 1.0f, 0.0f),   // 右上角 (索引 2)
    Vertex(0.5f, -0.5f, 0.0f, 1.0f, 1.0f)   // 右下角 (索引 3)
  };

  D3D11_BUFFER_DESC vbd{};
  vbd.Usage = D3D11_USAGE_IMMUTABLE; // 声明这些顶点永远不会变，显卡会把它放在最快的内存区
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
}

void SpriteRenderer::initRasterizerState()
{
  D3D11_RASTERIZER_DESC rd{};
  rd.FillMode = D3D11_FILL_SOLID;   // 实心填充
  rd.CullMode = D3D11_CULL_NONE;    // 关闭背面剔除
  rd.FrontCounterClockwise = FALSE; // 默认正面是顺时针
  rd.DepthClipEnable = TRUE;

  HRESULT hr = m_device->getDevice()->CreateRasterizerState(&rd, m_rasterizerState.GetAddressOf());
  LOG_DX11_CHECK(hr, "Failed to create Rasterizer State.");
}

} // namespace Graphics
