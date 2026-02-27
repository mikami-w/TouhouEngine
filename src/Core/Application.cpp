#include "Application.hpp"
#include "Graphics/DX11Device.hpp"
#include "Graphics/SpriteRenderer.hpp"
#include "Logger.hpp"
#include "MathUtils.hpp"
#include "Timer.hpp"
#include "Window.hpp"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <mmsystem.h>
#include <windows.h>

#include <memory>
#include <numbers>

namespace Core {

Application::Application(Config const& config)
  : m_config(config)
  , m_isRunning(true)
{
  LOG_INFO("Initializing application...");

  // 初始化窗口
  Window::Config wndConfig{ .title = m_config.title, .width = m_config.width, .height = m_config.height };
  m_window = std::make_unique<Window>(wndConfig);
  LOG_INFO("Display Resolution: " + std::to_string(m_config.width) + "x" + std::to_string(m_config.height));

  // 初始化图形设备 DirectX 11 (传入窗口句柄)
  m_gfx =
    std::make_unique<Graphics::DX11Device>(m_window->getHandle(), m_config.width, m_config.height, m_config.vsync);

  // 初始化计时器
  m_timer = std::make_unique<Timer>();

  // 初始化渲染管线
  m_spriteRenderer = std::make_unique<Graphics::SpriteRenderer>(m_gfx.get());
  m_spriteRenderer->initialize();
  m_spriteRenderer->updateProjectionMatrix(static_cast<float>(m_config.width), static_cast<float>(m_config.height));

  // 加载贴图
  auto texPath = std::filesystem::current_path() / "assets/textures/yukari.png";
  m_textureYukari = std::make_unique<Graphics::Texture>(m_gfx.get(), texPath.string());

  // 加载图集
  auto fontPath = std::filesystem::current_path() / "assets/textures/ui/fonts/ascii.png";
  m_fontTexture = std::make_unique<Graphics::Texture>(m_gfx.get(), fontPath.string());

  m_bulletManager.init(
    m_spriteRenderer->getMaxInstances()); // 初始化弹幕池, 最多支持的子弹数取决于 SpriteRenderer 的实例缓冲区大小

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
    ++m_frameCount;
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

      if (!m_config.vsync) {
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
      LOG_DEBUG(std::format("Current FPS: {:.2f}", m_fps));
    }
  }
}

void Application::update()
{
  {
    // 生成子弹
    static constexpr float spawnAngAccel = 0.001f;
    static float spawnAngle = 0.0f;
    static float spawnAngVel = 0.0f;
    spawnAngVel += spawnAngAccel; // 逐渐加速旋转
    spawnAngle += spawnAngVel;
    Game::Bullet b{ .x = m_config.width / 2.0f, .y = m_config.height / 2.0f };
    b.angle = spawnAngle;
    b.speed = 8.0f;

    // 调高循环次数 (如 100) 用于压力测试或测试处理落机制
    for (int i = 0; i < 1; i++) {
      // 每帧生成 3 颗子弹
      m_bulletManager.spawnBullet(b);
      b.angle += Math::PI2_3_f;
      m_bulletManager.spawnBullet(b);
      b.angle += Math::PI2_3_f;
      m_bulletManager.spawnBullet(b);
    }
  }

  // 更新子弹位置, 并回收出界子弹
  m_bulletManager.update(static_cast<float>(m_config.width), static_cast<float>(m_config.height));
}

void Application::render()
{
  // 为减少 drawcall 次数, 使用同一 texture 文件的绘制应相邻, 以便 SpriteRenderer 的批处理逻辑能把它们合并成一个
  // drawcall

  m_gfx->clear(0.3f, 0.0f, 0.3f, 1.0f); // 清屏(背景)
  m_spriteRenderer->begin();            // 开启渲染管线状态
  float time = static_cast<float>(m_timer->getTotalTime());

  Game::Bullet const* bullets = m_bulletManager.getActiveBullets();
  size_t count = m_bulletManager.getActiveCount();

  // 暂时复用八云紫的贴图来当做子弹, 缩小到 20x20
  for (size_t i = 0; i < count; i++) {
    Game::Bullet const& b = bullets[i];
    m_spriteRenderer->drawSprite(m_textureYukari.get(),
                                 b.x,
                                 b.y,
                                 b.angle - Math::PI_2_f, // 子弹总是面向运动方向
                                 30.0f,
                                 30.0f // 子弹大小
    );
  }

  // float x = std::sin(time) * 200.0f + 400.0f;
  // float y = std::sin(std::sin(time) * 3.14159f) * 200.0f + 300.0f;
  float angle = Math::sin(time) * 0.2f;
  float width = static_cast<float>(m_textureYukari->getWidth() >> 2);
  float height = static_cast<float>(m_textureYukari->getHeight() >> 2);

  m_spriteRenderer->drawSprite(m_textureYukari.get(),
                               static_cast<float>(m_config.width / 2),
                               static_cast<float>(m_config.height / 2),
                               angle,
                               -width,
                               height);

  // 绘制 fps
  std::string fpsStr = std::format("{:.2f}FPS", m_fps);
  m_spriteRenderer->drawText(m_fontTexture.get(), fpsStr, m_config.width - 150.0f, m_config.height - 40.0f, 1.0f);

  m_spriteRenderer->end(); // 结束渲染管线状态
  m_gfx->present();        // 呈现到屏幕
}
} // namespace Core
