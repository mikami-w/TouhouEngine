#include "Application.hpp"

#include <memory>

#include <mmsystem.h>
#include <windows.h>

#include "Game/Bullet.hpp"
#include "Game/World.hpp"
#include "Graphics/DX11Device.hpp"
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

  // 初始化窗口
  Window::Config wndConfig{ .title = m_config.window.title,
                            .width = m_config.window.width,
                            .height = m_config.window.height };
  m_window = std::make_unique<Window>(wndConfig);
  LOG_INFO("Display Resolution: " + std::to_string(m_config.window.width) + "x" +
           std::to_string(m_config.window.height));

  // 初始化图形设备 DirectX 11 (传入窗口句柄)
  m_gfx = std::make_unique<Graphics::DX11Device>(
    m_window->getHandle(), m_config.window.width, m_config.window.height, m_config.window.vsync);

  // 初始化计时器
  m_timer = std::make_unique<Timer>();

  // 初始化渲染管线
  m_spriteRenderer = std::make_unique<Graphics::SpriteRenderer>(m_gfx.get());
  m_spriteRenderer->initialize();

  // 加载纹理资源
  m_textureManager = std::make_unique<Graphics::TextureManager>(m_gfx.get());
  m_textureManager->loadTexture("yukari", "assets/textures/yukari.png");
  m_textureManager->loadTexture("ascii", "assets/textures/ui/fonts/ascii.png");

  // 初始化游戏世界, 并传入纹理管理器 (依赖注入)
  m_world = std::make_unique<Game::World>(m_textureManager.get());

  timeBeginPeriod(1); // 请求 1ms 的系统计时精度, 用于解决 Sleep(1) 的精度问题导致的微卡顿

  LOG_INFO("Application initialized successfully.");
}

Application::~Application()
{
  timeEndPeriod(1); // 恢复默认计时精度
  LOG_INFO("Application shutting down.");
}

void Application::run()
{
  m_timer->tick();
  double nextFrameTime = m_timer->getTotalTime() + SECONDS_PER_FRAME; // 计划下一帧的时间点

  // 主循环
  while (m_isRunning) {
    // 处理窗口消息, 如果窗口被关闭则停止循环
    if (!m_window->processMessages()) {
      m_isRunning = false;
      break;
    }
    // TODO: 添加其他退出逻辑

    update();
    render();
    ++m_fpsFrameCount;

    // Hybrid Spin-Wait (混合自旋等待)
    for (;;) {
      m_timer->tick();
      double currentTime = m_timer->getTotalTime();       // 当前时间
      double timeRemaining = nextFrameTime - currentTime; // 距离下一帧的剩余时间

      // 时间到了, 准备进入下一帧
      if (timeRemaining <= 0.0) {
        if (-timeRemaining <= SECONDS_PER_FRAME) {
          // 正常情况: 当前时间只超过下一帧的计划时间不到 1 帧, 就直接进入下一帧
          nextFrameTime += SECONDS_PER_FRAME;
        } else {
          // 处理落: 当前时间已经超过下一帧的计划时间大于 1 帧, 重置时间锚点, 放弃追赶
          nextFrameTime = currentTime + SECONDS_PER_FRAME;
        }

        break; // 打破自旋, 进入主循环下一个 iteration
      }

      if (!m_config.window.vsync) {
        if (timeRemaining > 0.002) {
          // 如果离下一帧还有 2ms 以上，让出 CPU; 否则就不 Sleep, 直接死循环等待以提高精度
          Sleep(1);
        }
      } else {
        // 如果开了 Vsync, Present 内部会阻塞, 不需要我们自己控制时间
        break;
      }
    }

    if (m_timer->getTotalTime() - m_fpsTimeAccumulator >= 1.0) {
      m_fps = static_cast<float>(m_fpsFrameCount / (m_timer->getTotalTime() - m_fpsTimeAccumulator));
      m_fpsFrameCount = 0;
      m_fpsTimeAccumulator = m_timer->getTotalTime();
      // LOG_DEBUG(std::format("Current FPS: {:.2f}", m_fps));
    }
  }
}

void Application::update()
{
  m_world->update(m_config.playArea);
}

void Application::render()
{
  // 为减少 drawcall 次数, 使用同一 texture 文件的绘制应相邻, 以便 SpriteRenderer 的批处理逻辑能把它们合并成一个
  // drawcall

  m_gfx->clear(0.0f, 0.0f, 0.0f, 0.0f); // 清屏(背景)
  m_spriteRenderer->begin();            // 开启渲染管线状态

  // 切换视口到游戏区域
  m_spriteRenderer->setView(m_config.playArea.offsetX,
                            m_config.playArea.offsetY,
                            m_config.playArea.logicalWidth,
                            m_config.playArea.logicalHeight);

  m_world->draw(m_spriteRenderer.get());

  // 下面这段原本是用来让八云紫在屏幕中心小幅摆动, 充当发弹者, 后面由EnemyManager负责
  Graphics::Texture* yukari = m_textureManager->getTexture("yukari");
  static long long timecount = 0;
  float time = static_cast<float>(timecount++);
  float angle = Math::sin(time / 30.0f) * 0.2f;
  float width = yukari->getWidth() / 4.0f;
  float height = yukari->getHeight() / 4.0f;
  m_spriteRenderer->drawSprite(
    yukari, { m_config.playArea.logicalWidth / 2, m_config.playArea.logicalHeight / 2 }, angle, { -width, height });

  // 切换视口到全屏, 绘制 fps
  m_spriteRenderer->setView(
    0, 0, static_cast<float>(m_config.window.width), static_cast<float>(m_config.window.height));
  std::string fpsStr = std::format("{:.2f}FPS", m_fps);
  m_spriteRenderer->drawText(m_textureManager->getTexture("ascii"),
                             fpsStr,
                             { m_config.window.width - 150.0f, m_config.window.height - 40.0f },
                             1.0f);

  m_spriteRenderer->end(); // 结束渲染管线状态
  m_gfx->present();        // 呈现到屏幕
}
} // namespace Core
