#pragma once

#include "Core/EngineSettings.hpp"

#include <memory>

namespace Core {
class Window;
class Timer;
}

namespace Graphics {
class Texture;
class DX11Device;
class TextureManager;
class SpriteRenderer;
}

namespace Game {
class World;
}

namespace Core {
class Application
{
public:
  explicit Application(Config::EngineSettings const& config);
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
  Config::EngineSettings m_config;
  bool m_isRunning;

  std::unique_ptr<Window> m_window;
  std::unique_ptr<Timer> m_timer;
  std::unique_ptr<Graphics::DX11Device> m_gfx;
  std::unique_ptr<Graphics::SpriteRenderer> m_spriteRenderer;
  std::unique_ptr<Graphics::TextureManager> m_textureManager;
  std::unique_ptr<Game::World> m_world;

  // FPS 计算
  float m_fps = 0.0f;
  int m_fpsFrameCount = 0;
  double m_fpsTimeAccumulator = 0.0;
};
} // namespace Core
