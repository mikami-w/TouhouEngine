#pragma once
#include "DX11Device.hpp"
#include "Shader.hpp"

#include <memory>

namespace Graphics {
class SpriteRenderer
{
public:
  explicit SpriteRenderer(DX11Device* device);  // 依赖注入: 需要 DX11Device 来创建资源
  ~SpriteRenderer() = default;

  SpriteRenderer(SpriteRenderer const&) = delete;
  SpriteRenderer& operator=(SpriteRenderer const&) = delete;

  void initialize(); // 初始化管线状态 (加载 Shaders)
  void begin();
  void end();

private:
  DX11Device* m_device; // 不管理生命周期

  // 管线核心状态
  std::unique_ptr<VertexShader> m_vertexShader;
  std::unique_ptr<PixelShader> m_pixelShader;
  std::unique_ptr<InputLayout> m_inputLayout;
};
} // namespace Graphics
