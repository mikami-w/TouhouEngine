#include "Window.hpp"
#include "StringUtils.hpp"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdexcept>

namespace Core {
Window::Window(Config const& config)
  : m_config(config)
  , m_hWnd(nullptr)
  , m_hInst(GetModuleHandle(nullptr))
{
  // 告诉 Windows 程序自己处理高 DPI, 不需要系统进行 DPI 缩放
  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

  m_className = stringToWstring(m_config.title) + L"_Class";

  registerWindowClass();
  createWindowInstance();
}

Window::~Window()
{
  if (m_hWnd) {
    DestroyWindow(m_hWnd);
  }
  UnregisterClass(m_className.c_str(), m_hInst);
}

bool Window::processMessages()
{
  MSG msg{};
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
    if (msg.message == WM_QUIT) {
      return false;
    }
    TranslateMessage(&msg); // 翻译虚拟键消息
    DispatchMessage(&msg);  // 将消息分发到窗口的消息处理函数
  }
  return true;
}

void Window::registerWindowClass()
{
  WNDCLASSEX wc{ 0 };
  wc.cbSize = sizeof(wc);
  wc.style = CS_OWNDC;             // 独立的设备上下文
  wc.lpfnWndProc = handleMsgSetup; // 初始消息处理函数
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = m_hInst;
  wc.hIcon = nullptr;
  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wc.hbrBackground = nullptr;
  wc.lpszMenuName = nullptr;
  wc.lpszClassName = m_className.c_str();
  wc.hIconSm = nullptr;

  ATOM atom = RegisterClassEx(&wc);

  // 如果注册失败且不是因为类已存在
  if (!atom && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
    throw std::runtime_error("Failed to register window class");
  }
}

void Window::createWindowInstance()
{
  RECT wr{ 0, 0, m_config.width, m_config.height };
  AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

  auto wTitle = stringToWstring(m_config.title);
  // 创建窗口
  m_hWnd = CreateWindowEx(0,
                          m_className.c_str(),
                          wTitle.c_str(),
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          wr.right - wr.left,
                          wr.bottom - wr.top,
                          nullptr,
                          nullptr,
                          m_hInst,
                          this);

  if (!m_hWnd) {
    throw std::runtime_error("Failed to create window instance");
  }

  ShowWindow(m_hWnd, SW_SHOWDEFAULT);
}

LRESULT Window::handleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  // WM_NCCREATE 是窗口创建时收到的最早消息之一
  if (msg == WM_NCCREATE) {
    // 从 CREATESTRUCT 中提取 this
    const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
    Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

    pWnd->m_hWnd = hWnd; // 在调用 handleMsg 之前初始化

    // 将 this 存储在 Win32 的用户数据区中
    SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));

    // 将消息处理函数切换为 Redirect
    SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::handleMsgRedirect));

    // 将消息转发给成员函数
    return pWnd->handleMsg(hWnd, msg, wParam, lParam);
  }
  // 如果不是 NCCREATE，就按默认处理
  return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT Window::handleMsgRedirect(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  // 从用户数据区取回 this
  Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
  // 转发给成员函数
  return pWnd->handleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::handleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
    case WM_CLOSE: {
      PostQuitMessage(0); // 发送退出消息
      return 0;
    }
    // 这里以后会添加键盘、鼠标的处理逻辑
    default:
      (void)this;
  }
  return DefWindowProc(hWnd, msg, wParam, lParam);
}
} // namespace Core
