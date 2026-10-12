#include "TextureManager.hpp"

#if defined(_MSC_VER)
  #pragma warning(push)
  #pragma warning(disable: 4244 4702)
#elif defined(__GNUC__) || defined(__clang__)
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wconversion"
  #pragma GCC diagnostic ignored "-Wunreachable-code"
#endif

#include <jsonpp/jsonpp.hpp>

#if defined(_MSC_VER)
  #pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
  #pragma GCC diagnostic pop
#endif

#include "Core/Logger.hpp"
#include "Texture.hpp"

namespace Graphics {

TextureManager::TextureManager(SDL_GPUDevice* device)
  : m_device(device)
{
  if (!m_device) {
    LOG_ERROR("Null GPU device passed to TextureManager.");
    throw std::invalid_argument("GPU device is null.");
  }

  // 创建所有纹理共享的采样器 (线性过滤, Clamp)
  SDL_GPUSamplerCreateInfo samplerInfo{};
  samplerInfo.min_filter = SDL_GPU_FILTER_LINEAR;
  samplerInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
  samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
  samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
  samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
  samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
  samplerInfo.enable_anisotropy = false;
  samplerInfo.enable_compare = false;
  samplerInfo.min_lod = 0.0f;
  samplerInfo.max_lod = 1000.0f;

  m_sampler = makeSampler(m_device, samplerInfo);
  if (!m_sampler) {
    LOG_ERROR(std::format("Failed to create shared sampler: {}", SDL_GetError()));
    throw std::runtime_error("Failed to create shared sampler.");
  }

  m_textures["404"] =
    std::make_unique<Texture>(m_device, m_sampler.get(), (std::filesystem::current_path() / "assets/textures/404.png").string());
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
  Texture* texp =
    (m_textures[name] = std::make_unique<Texture>(m_device, m_sampler.get(), fullTexturePath.string())).get();
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
