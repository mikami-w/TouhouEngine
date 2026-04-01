#include "TextureManager.hpp"

#pragma warning(push)
#pragma warning(disable : 4244) // 关闭隐式类型转换警告
#pragma warning(disable : 4702) // 关闭不可达代码警告

#include <jsonpp/jsonpp.hpp>

#pragma warning(pop)

#include "Core/Logger.hpp"
#include "DX11Device.hpp"
#include "Texture.hpp"

namespace Graphics {

TextureManager::TextureManager(DX11Device* device)
  : m_device(device)
{
  if (!m_device || !(m_device->getDevice())) {
    LOG_ERROR("Null DX11Device passed to TextureManager.");
    throw std::invalid_argument("DX11Device is null.");
  }
  m_textures["404"] =
    std::make_unique<Texture>(m_device, (std::filesystem::current_path() / "assets/textures/404.png").string());
  LOG_INFO("Texture Manager initialized with default '404' texture.");
}

void TextureManager::initTextures()
{
  loadTexture("tama_large", "assets/textures/tama_large.png");
  loadTexture("ascii", "assets/textures/ui/fonts/ascii.png");
}

Texture* TextureManager::loadTexture(std::string const& name, std::string_view filePath, std::string_view atlasJsonPath)
{
  using json = jsonpp::unordered_json;

  if (m_textures.contains(name)) {
    LOG_WARN("Texture with name '" + name + "' already exists. Overwriting.");
  }

  auto fullTexturePath = std::filesystem::current_path() / filePath;
  if (!std::filesystem::exists(fullTexturePath)) {
    LOG_ERROR("Texture file not found: " + fullTexturePath.string() + ". Failed to load texture.");
    return m_textures.at("404").get();
  }
  Texture* texp = (m_textures[name] = std::make_unique<Texture>(m_device, fullTexturePath.string())).get();
  LOG_INFO("Texture '" + name + "' loaded and stored in TextureManager.");

  if (!atlasJsonPath.empty()) {
    auto fullAtlasJsonPath = std::filesystem::current_path() / atlasJsonPath;
    if (!std::filesystem::exists(fullAtlasJsonPath)) {
      LOG_ERROR("Texture file not found: " + fullAtlasJsonPath.string() + ". Failed to load texture.");
      return texp;
    }
    json atlasJson = json::parse(atlasJsonPath);
    for (auto& [bulletName, metainfo] : atlasJson["frames"].as_object()) {
      m_sprites[name] = Sprite{ texp,
                                Sprite::UVRect{ .u = static_cast<float>(metainfo["frame"]["x"].as_float()),
                                                .v = static_cast<float>(metainfo["frame"]["y"].as_float()),
                                                .width = static_cast<float>(metainfo["frame"]["w"].as_float()),
                                                .height = static_cast<float>(metainfo["frame"]["h"].as_float()) } };
    }
  }

  return texp;
}

Texture* TextureManager::getTexture(std::string const& name) const
{
  auto it = m_textures.find(name);
  if (it != m_textures.end()) {
    return it->second.get();
  } else {
    LOG_WARN("Texture '" + name + "' is not loaded when attempting to get it.");
    return m_textures.at("404").get();
  }
}

void TextureManager::initMap()
{
  // TODO: 初始化
}
} // namespace Graphics
