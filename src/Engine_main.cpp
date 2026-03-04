#include "Core/Application.hpp"
#include "Core/ConfigManager.hpp"
#include "Core/Logger.hpp"
#include "Core/MathUtils.hpp"

using ArgFlag = unsigned int;
enum Mode : ArgFlag
{
  MODE_LOAD_BIN = 1u << 0,
  MODE_LOAD_JSON = 1u << 1,
  MODE_COOK = 1u << 2,
  MODE_HELP = 1u << 31
};

inline ArgFlag getArgs(int argc, char* argv[])
{
  if (argc == 1) {
    LOG_INFO("No command line arguments provided. Defaulting to binary config loading mode.");
    return MODE_LOAD_BIN;
  }

  std::vector<std::string_view> args(argv + 1, argv + argc);
  unsigned int modeFlag = 0;
  for (auto arg : args) {
    if (arg == "--cook") {
      modeFlag |= MODE_COOK;
    } else if (arg == "--load-bin-config") {
      modeFlag |= MODE_LOAD_BIN;
    } else if (arg == "--load-json-config") {
      modeFlag |= MODE_LOAD_JSON;
    } else if (arg == "--help") {
      modeFlag |= MODE_HELP;
    } else {
      LOG_ERROR(std::format("Unknown command line argument: '{}'. Use '--help' for usage information.", arg));
      throw std::runtime_error("Unknown command line argument.");
    }
  }

  if (!(modeFlag & MODE_LOAD_JSON || modeFlag & MODE_LOAD_BIN)) {
    LOG_INFO("No config loading mode specified. Defaulting to binary config.");
    modeFlag |= MODE_LOAD_BIN;
  }
  return modeFlag;
}

inline int handleArgs(ArgFlag modeFlag)
{
  if (modeFlag & MODE_HELP) {
    LOG_INFO("TouhouEngine CLI Usage:");
    LOG_INFO("  --load-bin-config or no args : Run game using compiled binary config (config.bin).");
    LOG_INFO("  --load-json-config           : Run game in Dev Mode using raw config.json.");
    LOG_INFO("  --cook-config                : Compile raw configs (JSON) to binary.");

    return 1;
  }

  if (modeFlag & MODE_COOK) {
    LOG_INFO("Cooking config from JSON to binary...");
    auto& cfgm = Core::Config::ConfigManager::getInstance();
    if (!cfgm.loadFromJson("config/config.json")) {
      LOG_ERROR("Failed to load config from JSON for cooking.");
      throw std::runtime_error("Failed to load config from JSON for cooking.");
    }
    if (!cfgm.saveToBinary("config/config.bin")) {
      LOG_ERROR("Failed to save cooked config to binary.");
      throw std::runtime_error("Failed to save cooked config to binary.");
    }
    LOG_INFO("Config cooked successfully. Output: config/config.bin");
  }

  if (modeFlag & MODE_LOAD_JSON && modeFlag & MODE_LOAD_BIN) {
    LOG_ERROR("Both --load-json-config and --load-bin-config flags are set. Please choose only one.");
    throw std::runtime_error("Conflicting config loading modes specified.");
  } else if (modeFlag & MODE_LOAD_JSON) {
    LOG_INFO("Running in Dev Mode with JSON config...");
    auto& cfgm = Core::Config::ConfigManager::getInstance();
    if (!cfgm.loadFromJson("config/config.json")) {
      LOG_ERROR("Failed to load config from JSON.");
      throw std::runtime_error("Failed to load config from JSON.");
    }
  } else if (modeFlag & MODE_LOAD_BIN) {
    LOG_INFO("Running with binary config...");
    auto& cfgm = Core::Config::ConfigManager::getInstance();
    if (!cfgm.loadFromBinary("config/config.bin")) {
      LOG_ERROR("Failed to load config from binary.");
      throw std::runtime_error("Failed to load config from binary.");
    }
  } else {
    LOG_WARN("No valid config loading mode specified. Use '--help' for usage information.");
  }
  return 0;
}

inline int initialize(int argc, char* argv[])
{
  constexpr char const* logFileName = "EngineLog.log";
  Core::Logger::getInstance().init(logFileName);

  if (handleArgs(getArgs(argc, argv)) == 1) {
    return 1; // 已处理 --help 参数, 直接退出
  }

  Core::Math::initMathUtils();

  return 0;
}

int main(int argc, char* argv[])
{
#if defined(_DEBUG)
  // 开启 CRT 内存泄漏检测
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  try {
    if (initialize(argc, argv) == 1) {
      return 0; // 已处理 --help 参数, 直接退出
    }

    Core::Application app{ Core::Config::ConfigManager::getInstance().getSettings() };
    app.run();
  } catch (std::exception& e) {
    LOG_FATAL(e.what());
    return -1;
  }

  return 0;
}
