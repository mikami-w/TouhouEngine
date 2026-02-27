#include "Core/Application.hpp"
#include "Core/Logger.hpp"
#include "Core/MathUtils.hpp"

int main(int argc, char* argv[])
{
  (void)argc, (void)argv;

#if defined(_DEBUG)
  // 开启 CRT 内存泄漏检测
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  constexpr char const* logFileName = "EngineLog.log";

  Core::Logger::getInstance().init(logFileName);
  Core::Math::initMathUtils();

  try {
    Core::Application::Config config{
      .title = "東方弾幕クリエイター ~ Touhou Engine Dev", .width = 1280, .height = 960, .vsync = false
    };
    Core::Application app{ config };
    app.run();
  } catch (std::exception& e) {
    LOG_FATAL(e.what());
    return -1;
  }

  return 0;
}
