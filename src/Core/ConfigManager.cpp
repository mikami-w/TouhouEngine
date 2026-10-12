#include "ConfigManager.hpp"

#include <fstream>

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

#include "Logger.hpp"

namespace Core::Config {

void ConfigManager::loadDefaults()
{
  m_settings = EngineSettings{};
}

bool ConfigManager::loadFromJson(std::string const& filePath)
{
  using namespace std::string_literals;
  using json = jsonpp::unordered_json;

  std::ifstream file(filePath);
  if (!file.is_open()) {
    LOG_ERROR("Failed to open config file: " + filePath);
    return false;
  }

  try {
    auto root = json::parse(file);

#if defined(_MSC_VER)
  #pragma warning(push)
  #pragma warning(disable : 4244) // 关闭隐式类型转换警告
#endif
    if (root.contains("Window")) {
      auto const& windowCfg = root["Window"];

      constexpr std::size_t BUFFER_SIZE = sizeof(m_settings.window.title); // 获取数组总大小 (256)
      std::string titleStr = windowCfg["title"].as_string();
      if (titleStr.size() > BUFFER_SIZE - 1) {
        LOG_WARN("Window title in config is too long and will be truncated to fit the buffer.");
      }
      strncpy(m_settings.window.title, titleStr.c_str(), BUFFER_SIZE - 1);
      m_settings.window.title[BUFFER_SIZE - 1] = 0;

      m_settings.window.width = windowCfg["width"].as_int();
      m_settings.window.height = windowCfg["height"].as_int();
      m_settings.window.vsync = windowCfg["vsync"].as_bool();
      m_settings.window.fullscreen = windowCfg["fullscreen"].as_bool();
    }
    if (root.contains("PlayArea")) {
      auto const& playAreaCfg = root["PlayArea"];

      m_settings.playArea.logicalWidth = playAreaCfg["logicalWidth"].as_float();
      m_settings.playArea.logicalHeight = playAreaCfg["logicalHeight"].as_float();
      m_settings.playArea.offsetX = playAreaCfg["offsetX"].as_float();
      m_settings.playArea.offsetY = playAreaCfg["offsetY"].as_float();
      m_settings.playArea.scale = playAreaCfg["scale"].as_float();
    }
    if (root.contains("Debug")) {
      auto const& debugCfg = root["Debug"];

      m_settings.debug.showFPS = debugCfg["showFPS"].as_bool();
      m_settings.debug.showHitbox = debugCfg["showHitbox"].as_bool();
    }
#if defined(_MSC_VER)
  #pragma warning(pop)
#endif
    LOG_INFO("Configuration loaded successfully from JSON.");
    return true;
  } catch (jsonpp::JsonParseError& e) {
    LOG_ERROR("json parsing error: "s + e.what());
    return false;
  } catch (jsonpp::JsonException& e) {
    LOG_ERROR("json error: "s + e.what());
    loadDefaults(); // 此时配置可能已经被破坏, 加载默认配置以保证程序能继续运行
    return false;
  } catch (std::exception& e) {
    LOG_ERROR(e.what());
    loadDefaults();
    throw;
  }
}

bool ConfigManager::loadFromBinary(std::string const& filePath)
{
  std::ifstream file(filePath, std::ios::binary);
  if (!file.is_open()) {
    LOG_ERROR("Failed to open binary config file: " + filePath);
    return false;
  }

  file.read(reinterpret_cast<char*>(&m_settings), sizeof(EngineSettings));
  LOG_INFO("Configuration loaded instantly from Binary.");
  return true;
}

bool ConfigManager::saveToBinary(std::string const& filePath)
{
  std::ofstream file(filePath, std::ios::binary | std::ios::trunc);
  if (!file.is_open()) {
    LOG_ERROR("Failed to create binary config file: " + filePath);
    return false;
  }

  file.write(reinterpret_cast<char const*>(&m_settings), sizeof(EngineSettings));
  return true;
}
} // namespace Core::Config
