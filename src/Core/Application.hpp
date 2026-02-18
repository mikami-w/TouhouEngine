#pragma once

#include "Graphics/SpriteRenderer.hpp"
#include "Graphics/Texture.hpp"

#include <memory>
#include <string>

namespace Core {
class Window;
class Timer;
}

namespace Graphics {
class DX11Device;
}

namespace Core {
class Application
{
public:
  struct Config
  {
    std::string title;
    int width;
    int height;
    bool vsync;
  };

public:
  explicit Application(Config const& config);
  ~Application();

  // 禁止拷贝和赋值
  Application(Application const&) = delete;
  Application& operator=(Application const&) = delete;

  void run(); // 启动应用程序主循环

public:
  static constexpr double TARGET_FPS = 60.0;
  static constexpr double SECONDS_PER_FRAME = 1.0 / TARGET_FPS; // 约为 0.0166667 秒

private:
  void update(); // 处理逻辑更新, 每帧调用
  void render(); // 处理渲染提交, 尽可能快, 或被 vsync 限制

private:
  Config m_config;
  bool m_isRunning;

  std::unique_ptr<Core::Window> m_window;
  std::unique_ptr<Graphics::DX11Device> m_gfx;
  std::unique_ptr<Core::Timer> m_timer;
  std::unique_ptr<Graphics::SpriteRenderer> m_spriteRenderer;
  std::unique_ptr<Graphics::Texture> m_textureYukari;
};
} // namespace Core
