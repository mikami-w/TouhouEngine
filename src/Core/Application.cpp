#include "Application.hpp"
#include "Graphics/DX11Device.hpp"
#include "Graphics/SpriteRenderer.hpp"
#include "Logger.hpp"
#include "MathUtils.hpp"
#include "Timer.hpp"
#include "Window.hpp"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

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

  m_bulletManager.init(20000); // 初始化弹幕池, 最多支持 20000 发子弹

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
  double accumulatedTime = 0.0; // 累积的未处理时间
  // 主循环
  // 处理窗口消息, 如果窗口被关闭则停止循环
  while (m_isRunning) {
    if (!m_window->processMessages()) {
      m_isRunning = false;
      break;
    }
    // TODO: 添加其他退出逻辑

    // 获取当前经过的时间, 并累积到 accumulatedTime 中
    m_timer->tick();

    double dt = m_timer->getDeltaTime();
    accumulatedTime += dt;

    // 逻辑更新 (Update)
    // 只有累积的时间经过了一帧的时间 (1/60s), 才执行一次更新循环
    // 若由于卡顿等原因 accumulatedTime 积累了 2 帧或更多,
    // 则连续执行多次更新循环而不渲染, 以追赶上当前时间
    bool isUpdated = false;
    int updateCount = 0; // 统计连续更新的次数, 用于限制最大连续更新次数, 模拟处理落机制
    while (accumulatedTime >= SECONDS_PER_FRAME &&
           updateCount++ < 2) { // 最多连续更新 2 次, 即通过处理落机制最多降低到 30fps
      update();
      accumulatedTime -= SECONDS_PER_FRAME; // 减去一帧的时间
      isUpdated = true;
    }

    // 计算 fps
    m_fpsTimeAccumulator += dt;
    if (m_fpsTimeAccumulator >= 0.5f) {
      m_fps = static_cast<float>(1.0 * m_fpsFrameCount / m_fpsTimeAccumulator);
      m_fpsFrameCount = 0;
      m_fpsTimeAccumulator -= 0.5;
      LOG_DEBUG(std::format("Current FPS: {}", m_fps));
    }

    // 渲染提交 (Render)
    // 只有逻辑更新过 (画面有变化) 才重新渲染
    if (isUpdated) {
      render();
    } else if (!m_config.vsync) {
      // 如果没有更新, 就等一会儿再渲染, 避免 CPU 占用过高
      Sleep(1);
    }
  }
}

void Application::update()
{
  ++m_frameCount;
  ++m_fpsFrameCount;

  static constexpr float spawnAngAccel = 0.001f;
  static float spawnAngle = 0.0f;
  static float spawnAngVel = 0.0f;
  spawnAngVel += spawnAngAccel; // 逐渐加速旋转
  spawnAngle += spawnAngVel;
  Game::Bullet b{ .x = m_config.width / 2.0f, .y = m_config.height / 2.0f };
  b.angle = spawnAngle;
  b.speed = 8.0f;

  // 每帧生成 3 颗子弹
  m_bulletManager.spawnBullet(b);
  b.angle += Math::PI2_3_f;
  m_bulletManager.spawnBullet(b);
  b.angle += Math::PI2_3_f;
  m_bulletManager.spawnBullet(b);

  // 更新子弹位置, 并回收出界子弹
  m_bulletManager.update(static_cast<float>(m_config.width), static_cast<float>(m_config.height));
}

void Application::render()
{
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
  float width = m_textureYukari->getWidth() >> 2;
  float height = m_textureYukari->getHeight() >> 2;

  m_spriteRenderer->drawSprite(m_textureYukari.get(), m_config.width / 2, m_config.height / 2, angle, -width, height);

  // 绘制 fps
  std::string fpsStr = std::format("{:.1f}FPS", m_fps);
  m_spriteRenderer->drawText(m_fontTexture.get(), fpsStr, m_config.width - 150.0f, m_config.height - 40.0f, 1.0f);

  m_spriteRenderer->end(); // 结束渲染管线状态
  m_gfx->present();        // 呈现到屏幕

  // LOG_DEBUG(std::format("Active Bullets: {}", m_bulletManager.getActiveCount()));
}
} // namespace Core
