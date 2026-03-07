#pragma once

#include "Shader.hpp"
#include "Vertex.hpp"

#include <DirectXMath.h>
#include <memory>
#include <vector>

namespace Graphics {
class DX11Device;
class Texture;
}

namespace Graphics {
// 常量缓冲区
struct TransformCBuffer
{
  DirectX::XMFLOAT4X4 projection; // 投影矩阵
};

class SpriteRenderer
{
public:
  explicit SpriteRenderer(DX11Device* device); // 依赖注入: 需要 DX11Device 来创建资源
  ~SpriteRenderer() = default;

  SpriteRenderer(SpriteRenderer const&) = delete;
  SpriteRenderer& operator=(SpriteRenderer const&) = delete;

  std::size_t getMaxInstances() const noexcept { return m_maxInstances; }

  void initialize(); // 初始化管线状态 (加载 Shaders)

  void setView(float screenX,
               float screenY,
               float screenW,
               float screenH,
               float logicalW = -1.0f,
               float logicalH = -1.0f);

  void begin(); // 开始渲染当前帧的 Sprites
  void end();   // 结束渲染

  // pos 屏幕像素坐标, angle 弧度, scale 图像宽高像素大小
  void drawSprite(Texture* texture,
                  Core::Position pos,
                  float angle,
                  Core::Vector2 scale,
                  Core::Vector4 uvRect = { 0.0f, 0.0f, 1.0f, 1.0f }, // (u, v, width, height), 默认使用全图
                  Core::Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f }   // RGBA 颜色, 默认白色滤镜 (原图颜色)
  );
  void drawText(Texture* texture,
                std::string const& text,
                Core::Vector2 start,
                float scale = 1.0f,
                Core::Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f });

private:
  void initShaders();
  void initBuffers();
  void initStates();

  void flush();

private:
  DX11Device* m_device; // 不管理生命周期

  // 管线核心状态
  std::unique_ptr<VertexShader> m_vertexShader;
  std::unique_ptr<PixelShader> m_pixelShader;
  std::unique_ptr<InputLayout> m_inputLayout;

  Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;   // 顶点缓冲区
  Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;    // 索引缓冲区
  Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer; // 常量缓冲区
  Microsoft::WRL::ComPtr<ID3D11Buffer> m_instanceBuffer; // 实例缓冲区

  Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState; // 光栅化状态
  Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;       // 采样器状态
  Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState;           // 混合状态

  // 缓存的投影矩阵 (只要窗口大小不变, 投影矩阵就不变)
  DirectX::XMFLOAT4X4 m_projectionMatrix;

  // 批处理数据
  std::vector<InstanceData> m_instances; // 存储当前帧所有待渲染的 Sprite 实例数据
  Texture* m_currentTexture = nullptr;   // 当前批次使用的贴图
  std::size_t m_maxInstances = 50000;    // 最大 Sprite 实例数量
};
} // namespace Graphics
