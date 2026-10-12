#pragma once

#include <vector>

#include <SDL3/SDL_gpu.h>

#include "GPUResourceManager.hpp"
#include "Vertex.hpp"

namespace Graphics {
class GFXDevice;
class Texture;
}

namespace Graphics {

struct TransformUniform
{
  float projection[16]; // 4x4 投影矩阵, 行主序
};

class SpriteRenderer
{
public:
  explicit SpriteRenderer(GFXDevice* gfx);
  ~SpriteRenderer() = default;

  SpriteRenderer(SpriteRenderer const&) = delete;
  SpriteRenderer& operator=(SpriteRenderer const&) = delete;

  std::size_t getMaxInstances() const noexcept { return m_maxInstances; }

  void initialize(SDL_Window* window); // 创建 PSO + Buffers + Sampler

  void setView(float screenX,
               float screenY,
               float screenW,
               float screenH,
               float logicalW = -1.0f,
               float logicalH = -1.0f);

  // 帧生命周期
  void beginFrame(SDL_Window* window); // 获取 command buffer + swapchain + 开始 render pass
  void begin();                         // 推送 uniform
  void drawSprite(Texture* texture,
                  Core::Vector2 pos,
                  float angle,
                  Core::Vector2 scale,
                  Core::Vector4 uvRect = { 0.0f, 0.0f, 1.0f, 1.0f },
                  Core::Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f });
  void drawText(Texture* texture,
                std::string const& text,
                Core::Vector2 start,
                float scale = 1.0f,
                Core::Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f });
  void end();                            // flush 剩余实例
  void endFrame();                       // 结束 render pass, 提交 command buffer

private:
  void initShaders(SDL_Window* window);
  void initPipeline(SDL_Window* window);
  void initBuffers();
  void initSampler();

  void flush();

private:
  GFXDevice* m_gfx; // 非拥有

  // 着色器
  ShaderPtr m_vertexShader;
  ShaderPtr m_fragmentShader;

  // PSO (管线状态对象)
  PipelinePtr m_pipeline;

  // GPU 缓冲区
  GPUBufferPtr m_vertexBuffer;
  GPUBufferPtr m_indexBuffer;
  GPUBufferPtr m_instanceBuffer;
  TransferBufPtr m_instanceTransferBuffer;

  // 采样器
  GPUSamplerPtr m_sampler;

  // 帧状态
  SDL_GPUCommandBuffer* m_cmd = nullptr;
  SDL_GPURenderPass* m_renderPass = nullptr;
  SDL_GPUTexture* m_swapchainTexture = nullptr;
  Uint32 m_swapchainWidth = 0;
  Uint32 m_swapchainHeight = 0;

  // 投影矩阵
  float m_projectionMatrix[16];

  // 视口参数 (用于 setView 的 deferred 逻辑)
  float m_viewX = 0, m_viewY = 0, m_viewW = 0, m_viewH = 0;

  // 批处理数据
  std::vector<InstanceData> m_instances;
  Texture* m_currentTexture = nullptr;
  std::size_t m_maxInstances = 50000;
};
} // namespace Graphics
