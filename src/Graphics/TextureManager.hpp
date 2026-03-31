#pragma once

#include "SpriteType.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace Graphics {
class DX11Device;
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
  explicit TextureManager(DX11Device* device);
  ~TextureManager() = default;

  TextureManager(TextureManager const&) = delete;
  TextureManager& operator=(TextureManager const&) = delete;

  void initTextures();

  Texture* loadTexture(std::string const& name, std::string_view filePath, std::string_view atlasJsonPath = "");
  Texture* getTexture(std::string const& name) const;

  void clear();

private:
  static void initMap();

private:
  DX11Device* m_device;
  std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures; // 纹理资源池
  std::unordered_map<std::string, Sprite> m_sprites;

  static std::unordered_map<std::string, SpriteType> s_spriteMap; // 名称字符串到枚举的映射
};
} // namespace Graphics
