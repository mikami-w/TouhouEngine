#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <memory>
#include <string>

namespace Core {
class Window
{
public:
  struct Config
  {
    std::string title;
    int width;
    int height;
  };

public:
  explicit Window(Config const& config);
  ~Window();
  Window(Window const&) = delete; // 不可拷贝
  Window& operator=(Window const&) = delete;

  bool processMessages();                   // 处理消息队列, 返回 false 表示收到 exit 信息, 每帧调用
  HWND getHandle() const { return m_hWnd; } // 获取原生句柄

private:
  void registerWindowClass();  // 注册窗口类
  void createWindowInstance(); // 创建窗口实例

  // 初始 (第一次) 消息处理函数
  static LRESULT CALLBACK handleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
  // 后续消息处理函数
  static LRESULT CALLBACK handleMsgRedirect(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

  // 处理窗口消息的成员函数
  LRESULT handleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
  Config m_config;
  HWND m_hWnd;              // 窗口句柄
  HINSTANCE m_hInst;        // 应用程序实例句柄
  std::wstring m_className; // 窗口类名
};
} // namespace Core
