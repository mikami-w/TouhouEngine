#include "Application.hpp"
#include "Window.hpp"
#include "Graphics/DX11Device.hpp"
#include "Timer.hpp"
#include "Logger.hpp"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <memory>

namespace Core {

Application::Application(Config const& config)
  : m_config(config), m_isRunning(true)
{
  LOG_INFO("Initializing application...");

  // 初始化窗口
  Window::Config wndConfig{
    .title = m_config.title,
    .width = m_config.width,
    .height = m_config.height
  };
  m_window = std::make_unique<Window>(wndConfig);

  // 初始化图形设备 DirectX 11 (传入窗口句柄)
  m_gfx = std::make_unique<Graphics::DX11Device>(
    m_window->getHandle(),
    m_config.width,
    m_config.height
    );
  m_gfx->setVsync(m_config.vsync);

  // 初始化计时器
  m_timer = std::make_unique<Timer>();

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
    // 若由于卡顿等原因 accumulatedTime 积累了 2 帧或更多, 则连续执行多次更新循环而不渲染, 以追赶上当前时间
    bool isUpdated = false;
    while (accumulatedTime >= SECONDS_PER_FRAME) {
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
  // TODO: 引擎的核心逻辑 (更新虚拟机、子弹位置、处理碰撞) 在这里调用
}

void Application::render()
{

  m_gfx->clear(0.5f, 0.0f, 0.5f, 1.0f); // 清屏

  // TODO: 通知 SpriteBatch 进行批量绘制

  m_gfx->present(); // 呈现到屏幕
}
} // namespace Core
