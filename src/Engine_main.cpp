#include <iostream>

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
    Core::Window::Config config{
      .title = "東方弾幕クリエイター ~ Touhou Engine Dev",
      .width = 800,
      .height = 600
    };

    Core::Window wnd{ config };
    Graphics::DX11Device gfx{ wnd.getHandle(), config.width, config.height };
    gfx.setVsync(false);

    constexpr double TARGET_FPS = 60.0;
    constexpr double SECONDS_PER_FRAME = 1.0 / TARGET_FPS; // 约为 0.0166667 秒

    Core::Timer timer;
    double accumulatedTime = 0.0; // 累积的未处理时间

    // 主循环
    while (wnd.processMessages()) {
      // 获取当前经过的时间, 并累积到 accumulatedTime 中
      timer.tick();
      accumulatedTime += timer.getDeltaTime();

      // 逻辑更新 (Update)
      // 只有累积的时间经过了一帧的时间 (1/60s), 才执行一次更新循环
      // 若由于卡顿等原因 accumulatedTime 积累了 2 帧或更多, 则连续执行多次更新循环而不渲染, 以追赶上当前时间
      bool isUpdated = false;
      while (accumulatedTime >= SECONDS_PER_FRAME) {
        // TODO: 未来在这里执行 VM 脚本、移动子弹: VM.Execute(), BulletManager.Update()

        accumulatedTime -= SECONDS_PER_FRAME; // 减去一帧的时间
        isUpdated = true;
      }

      // 渲染提交 (Render)
      // 只有逻辑更新过 (画面有变化) 才重新渲染
      if (isUpdated) {
        gfx.clear(0.5f, 0.0f, 0.5f, 1.0f);

        // TODO: 未来在这里调用 SpriteBatch 绘制子弹和 Boss: SpriteBatch.Draw()

        gfx.present();
      } else {
        // 如果没有更新, 就等一会儿再渲染, 避免 CPU 占用过高
        Sleep(1);
      }
    }
  } catch (std::exception& e) {
    LOG_FATAL(e.what());
    MessageBoxA(nullptr, e.what(), "Error", MB_OK | MB_ICONERROR);
    return -1;
  }

  return 0;
}
