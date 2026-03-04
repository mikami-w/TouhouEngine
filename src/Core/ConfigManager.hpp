#pragma once

#include "EngineSettings.hpp"
#include "Logger.hpp"

#include <string>

namespace Core::Config {
class ConfigManager
{
public:
  static ConfigManager& getInstance()
  {
    static ConfigManager instance;
    return instance;
  }

  // 禁用拷贝与赋值
  ConfigManager(ConfigManager const&) = delete;
  ConfigManager& operator=(ConfigManager const&) = delete;

  EngineSettings const& getSettings() const noexcept { return m_settings; }
  void loadDefaults();

  bool loadFromJson(std::string const& filePath);
  bool loadFromBinary(std::string const& filePath);
  bool saveToBinary(std::string const& filePath);

private:
  ConfigManager() = default;
  ~ConfigManager() = default;

  EngineSettings m_settings;
};
} // namespace Core::Config
