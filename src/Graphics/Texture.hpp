#pragma once

#include <SDL3/SDL_gpu.h>
#include <string>

#include "GPUResourceManager.hpp"

namespace Graphics {

class Texture
{
public:
  // 从文件加载 (PNG/JPG 等), 上传到 GPU
  Texture(SDL_GPUDevice* device, SDL_GPUSampler* sampler, std::string const& filePath);
  ~Texture() = default;

  Texture(Texture const&) = delete;
  Texture& operator=(Texture const&) = delete;

  SDL_GPUTexture* getTexture() const { return m_texture.get(); }
  SDL_GPUSampler* getSampler() const { return m_sampler; }

  // 获取纹理+采样器绑定 (用于 render pass 中绑定)
  SDL_GPUTextureSamplerBinding getBinding() const;

  int getWidth() const { return m_width; }
  int getHeight() const { return m_height; }

private:
  GPUTexturePtr m_texture;
  SDL_GPUSampler* m_sampler; // 非拥有, 由 TextureManager 统一管理
  int m_width = 0;
  int m_height = 0;
};

} // namespace Graphics
