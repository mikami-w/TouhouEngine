#include <iostream>

#include "Core/Application.hpp"
#include "Core/Timer.hpp"
#include "Core/Logger.hpp"
#include "Core/Window.hpp"
#include "Graphics/DX11Device.hpp"

int main(int argc, char* argv[])
{
#if defined(_DEBUG)
  // 开启 CRT 内存泄漏检测
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  try {
    Core::Application::Config config{
      .title = "東方弾幕クリエイター ~ Touhou Engine Dev",
      .width = 800,
      .height = 600,
      .vsync = false
    };

    Core::Application app{ config };
    app.run();

  } catch (std::exception& e) {
    LOG_FATAL(e.what());
    return -1;
  }

  return 0;
}
