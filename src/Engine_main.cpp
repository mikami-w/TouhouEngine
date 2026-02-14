#include <iostream>

#include "Core/Window.hpp"

int main(int argc, char* argv[])
{
#if defined(_DEBUG)
  // 开启 CRT 内存泄漏检测
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  try {
    Core::Window::Config config{
      .title = "東方弾幕クリエイター ~ Touhou Engine Dev",
      .width = 800,
      .height = 600
    };

    Core::Window wnd{ config };

    for (;;) {
      // 处理窗口消息
      if (!wnd.processMessages()) {
        break; // 收到退出消息，跳出循环
      }

      // 游戏逻辑更新
      // 渲染
    }

  } catch (std::exception& e) {
    std::cerr << "Exception thrown: " << e.what() << std::endl;
    MessageBoxA(nullptr, e.what(), "Error", MB_OK | MB_ICONERROR);
    return -1;
  }

  return 0;
}
