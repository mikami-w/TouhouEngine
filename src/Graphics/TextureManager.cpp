#include "TextureManager.hpp"
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

Texture* TextureManager::loadTexture(std::string const& name, std::string const& filePath)
{
  if (m_textures.contains(name)) {
    LOG_WARN("Texture with name '" + name + "' already exists. Overwriting.");
  }

  auto texPath = std::filesystem::current_path() / filePath;
  if (!std::filesystem::exists(texPath)) {
    LOG_ERROR("Texture file not found: " + texPath.string() + ". Failed to load texture.");
    return m_textures.at("404").get();
  }
  Texture* re = (m_textures[name] = std::make_unique<Texture>(m_device, texPath.string())).get();
  LOG_INFO("Texture '" + name + "' loaded and stored in TextureManager.");
  return re;
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
} // namespace Graphics
