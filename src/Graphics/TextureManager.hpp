#pragma once

#include "SpriteType.hpp"
#include "GPUResourceManager.hpp"

#include <memory>
#include <string>
#include <unordered_map>

#include <SDL3/SDL_gpu.h>

namespace Graphics {
class Texture;
}

namespace Graphics {
struct Sprite
{
  Texture* texture{}; // 纹理资源
  struct UVRect
  {
    float u = 0.0f;      // 纹理坐标起点 u
    float v = 0.0f;      // 纹理坐标起点 v
    float width = 1.0f;  // 纹理坐标宽度
    float height = 1.0f; // 纹理坐标高度
  } uvRect{};            // 纹理坐标矩形, 默认值为全图
};

class TextureManager
{
public:
  explicit TextureManager(SDL_GPUDevice* device);
  ~TextureManager() = default;

  TextureManager(TextureManager const&) = delete;
  TextureManager& operator=(TextureManager const&) = delete;

  void initTextures();

  Texture* loadTexture(std::string const& name, std::string_view filePath, std::string_view atlasJsonPath = "");
  Texture* getTexture(std::string const& name) const;

  SDL_GPUSampler* getSampler() const { return m_sampler.get(); }

  void clear();

private:
  static void initMap();

private:
  SDL_GPUDevice* m_device; // 非拥有
  GPUSamplerPtr m_sampler; // 所有纹理共享
  std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;
  std::unordered_map<std::string, Sprite> m_sprites;

  static std::unordered_map<std::string, SpriteType> s_spriteMap;
};
} // namespace Graphics
