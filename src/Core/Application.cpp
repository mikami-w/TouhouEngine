#include "Application.hpp"

#include <SDL3/SDL.h>

#include "Game/World.hpp"
#include "Graphics/GFXDevice.hpp"
#include "Graphics/SpriteRenderer.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/TextureManager.hpp"
#include "Logger.hpp"
#include "MathUtils.hpp"
#include "Timer.hpp"
#include "Window.hpp"

namespace Core {

Application::Application(Config::EngineSettings const& config)
  : m_config(config)
  , m_isRunning(true)
{
  LOG_INFO("Initializing application...");

  // 初始化SDL (如果尚未初始化)
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    LOG_ERROR(std::format("SDL_Init failed: {}", SDL_GetError()));
    throw std::runtime_error("SDL_Init failed.");
  }

  // 1. 初始化窗口
  Window::Config wndConfig{ .title = m_config.window.title,
                            .width = m_config.window.width,
                            .height = m_config.window.height };
  m_window = std::make_unique<Window>(wndConfig);
  LOG_INFO("Display Resolution: " + std::to_string(m_config.window.width) + "x" +
           std::to_string(m_config.window.height));

  // 2. 初始化GPU 设备
  m_gfx = std::make_unique<Graphics::GFXDevice>(m_window->getHandle(), m_config.window.vsync);

  // 3. 初始化渲染管线 (需要swapchain format, 所以在 GFXDevice 之后)
  m_spriteRenderer = std::make_unique<Graphics::SpriteRenderer>(m_gfx.get());
  m_spriteRenderer->initialize(m_window->getHandle());

  // 4. 加载纹理资源 (需要GPU 设备)
  m_textureManager = std::make_unique<Graphics::TextureManager>(m_gfx->getDevice());
  m_textureManager->initTextures();

  // 5. 初始化计时器
  m_timer = std::make_unique<Timer>();

  // 6. 初始化游戏世界 并传入纹理管理器 (依赖注入)
  m_world = std::make_unique<Game::World>(m_textureManager.get());

  LOG_INFO("Application initialized successfully.");
}

Application::~Application()
{
  LOG_INFO("Application shutting down.");
}

void Application::run()
{
  m_timer->tick();
  double nextFrameTime = m_timer->getTotalTime() + SECONDS_PER_FRAME;

  // 主循环
  while (m_isRunning) {
    // 处理 SDL 事件
    processEvents();
    if (!m_isRunning) {
      break;
    }

    update();
    render();
    ++m_fpsFrameCount;

    // Hybrid Spin-Wait (混合自旋等待)
    for (;;) {
      m_timer->tick();
      double currentTime = m_timer->getTotalTime();
      double timeRemaining = nextFrameTime - currentTime;

      if (timeRemaining <= 0.0) {
        if (-timeRemaining <= SECONDS_PER_FRAME) {
          nextFrameTime += SECONDS_PER_FRAME;
        } else {
          nextFrameTime = currentTime + SECONDS_PER_FRAME;
        }
        break;
      }

      if (!m_config.window.vsync) {
        if (timeRemaining > 0.002) {
          // 离下一帧还有2ms 以上, 让出 CPU
          SDL_DelayNS(1'000'000);
        }
      } else {
        // Vsync 的 Present 内部会阻塞
        break;
      }
    }

    // FPS 计算
    if (m_timer->getTotalTime() - m_fpsTimeAccumulator >= 1.0) {
      m_fps = static_cast<float>(m_fpsFrameCount / (m_timer->getTotalTime() - m_fpsTimeAccumulator));
      m_fpsFrameCount = 0;
      m_fpsTimeAccumulator = m_timer->getTotalTime();
      // LOG_DEBUG(std::format("Current FPS: {:.2f}", m_fps));
    }
  }
}

void Application::processEvents()
{
  SDL_Event event{};
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_EVENT_QUIT:
        m_isRunning = false;
        break;
      case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        m_isRunning = false;
        break;
      case SDL_EVENT_KEY_DOWN:
        if (event.key.key == SDLK_ESCAPE) {
          m_isRunning = false;
        }
        break;
      default:
        break;
    }
  }
}

void Application::update()
{
  m_world->update(m_config.playArea);
}

void Application::render()
{
  auto* window = m_window->getHandle();

  // 帧开始: 获取 command buffer + swapchain + 清屏
  m_spriteRenderer->beginFrame(window);

  // 开始渲染管线 (推送projection uniform)
  m_spriteRenderer->begin();

  // 切换视口到游戏区域
  m_spriteRenderer->setView(m_config.playArea.offsetX,
                            m_config.playArea.offsetY,
                            m_config.playArea.logicalWidth,
                            m_config.playArea.logicalHeight);

  m_world->draw(m_spriteRenderer.get());

  // 切换视口到全屏, 绘制 fps
  m_spriteRenderer->setView(
    0, 0, static_cast<float>(m_config.window.width), static_cast<float>(m_config.window.height));
  std::string fpsStr = std::format("{:.2f}FPS", m_fps);
  m_spriteRenderer->drawText(m_textureManager->getTexture("ascii"),
                             fpsStr,
                             { m_config.window.width - 150.0f, m_config.window.height - 40.0f },
                             1.0f);

  // 结束渲染, 提交 command buffer
  m_spriteRenderer->end();
  m_spriteRenderer->endFrame();
}

} // namespace Core
