#pragma once

#include "Core/Window.hpp"

#include <d3d11.h>
#include <wrl/client.h>

namespace Graphics {
class DX11Device
{
public:
  DX11Device(HWND hWnd, int width, int height);
  ~DX11Device() = default; // 使用默认析构函数，Microsoft::WRL::ComPtr 会自动释放资源

  // 禁止拷贝和赋值, 独占显卡资源
  DX11Device(DX11Device const&) = delete;
  DX11Device& operator=(DX11Device const&) = delete;

  void clear(float r, float g, float b, float a); // 清屏函数，接受 RGBA 颜色值
  void present();                                 // 显示当前帧, 即将后台缓冲区内容翻转到前台显示

  bool getVsync() const { return m_vsync; }
  void setVsync(bool enable) { m_vsync = enable ? 1 : 0; }

  ID3D11Device* getDevice() const { return m_device.Get(); }
  ID3D11DeviceContext* getContext() const { return m_context.Get(); }

private:
  Microsoft::WRL::ComPtr<ID3D11Device> m_device;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
  Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;

  unsigned m_vsync{ 0 };
};
} // namespace Graphics
