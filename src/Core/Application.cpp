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

  m_bulletManager.init(20000); // 初始化弹幕池, 最多支持 20000 发子弹

  LOG_INFO("Application initialized successfully.");
}

Application::~Application()
{
  LOG_INFO("Application shutting down.");
}

void Application::run()
{
  double accumulatedTime = 0.0; // 累积的未处理时间
  // 主循环
  while (m_isRunning) {
    if (!m_window->processMessages()) {
      m_isRunning = false;
      break;
    }
    // TODO: 添加其他退出逻辑

    // 处理窗口消息, 如果窗口被关闭则停止循环
    // 获取当前经过的时间, 并累积到 accumulatedTime 中
    m_timer->tick();
    accumulatedTime += m_timer->getDeltaTime();

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

  if (m_frameCount & 1) {
    // 每两帧生成一颗子弹, 以测试 BulletManager 的性能
    float angle = static_cast<float>(m_timer->getTotalTime());
    int speed = 10;
    Game::Bullet bullet{ .x = static_cast<float>(m_config.width / 2),
                         .y = static_cast<float>(m_config.height / 2),
                         .vx = speed * Math::cos(angle),
                         .vy = speed * Math::sin(angle),
                         .type = 0,
                         .color = 0 };
    m_bulletManager.spawnBullet(bullet);
  }
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
                                 0.0f, // 子弹暂不旋转
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

  m_spriteRenderer->end(); // 结束渲染管线状态
  m_gfx->present();        // 呈现到屏幕

  // LOG_DEBUG(std::format("Active Bullets: {}", m_bulletManager.getActiveCount()));
}
} // namespace Core
