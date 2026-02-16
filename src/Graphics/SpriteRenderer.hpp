#pragma once
#include "DX11Device.hpp"
#include "Shader.hpp"

#include <DirectXMath.h>
#include <memory>

namespace Graphics {
// 常量缓冲区
struct TransformCBuffer
{
  DirectX::XMFLOAT4X4 projection; // 投影矩阵
  DirectX::XMFLOAT4X4 world;      // 世界矩阵
};

class SpriteRenderer
{
public:
  explicit SpriteRenderer(DX11Device* device); // 依赖注入: 需要 DX11Device 来创建资源
  ~SpriteRenderer() = default;

  SpriteRenderer(SpriteRenderer const&) = delete;
  SpriteRenderer& operator=(SpriteRenderer const&) = delete;

  void initialize(); // 初始化管线状态 (加载 Shaders)

  // 动态更新投影矩阵 (当窗口大小改变时调用)
  void updateProjectionMatrix(float windowWidth, float windowHeight);

  void begin(); // 开始渲染当前帧的 Sprites
  void end();   // 结束渲染

  // x, y 是屏幕像素坐标, angle 是弧度, scaleX/Y 是宽高像素大小
  void drawTestQuad(float x, float y, float angle, float scaleX, float scaleY);

private:
  void initShaders();
  void initBuffers();
  void initRasterizerState();

private:
  DX11Device* m_device; // 不管理生命周期

  // 管线核心状态
  std::unique_ptr<VertexShader> m_vertexShader;
  std::unique_ptr<PixelShader> m_pixelShader;
  std::unique_ptr<InputLayout> m_inputLayout;

  Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;   // 顶点缓冲区
  Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;    // 索引缓冲区
  Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer; // 常量缓冲区

  Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState; // 光栅化状态对象

  // 缓存的投影矩阵 (只要窗口大小不变, 投影矩阵就不变)
  DirectX::XMFLOAT4X4 m_projectionMatrix;
};
} // namespace Graphics
