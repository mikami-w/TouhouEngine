#include "SpriteRenderer.hpp"
#include "Core/Logger.hpp"

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
  LOG_INFO("SpriteRenderer Pipeline initialized successfully.");
}

void SpriteRenderer::begin()
{
  auto context = m_device->getContext();

  // 将状态绑定到渲染管线
  m_inputLayout->Bind(context);
  m_vertexShader->Bind(context);
  m_pixelShader->Bind(context);

  // 拓扑结构: 告诉 GPU 传来的是一系列三角形
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void SpriteRenderer::end()
{
  // 目前暂不需要清理状态, 留作扩展口
}
} // namespace Graphics
