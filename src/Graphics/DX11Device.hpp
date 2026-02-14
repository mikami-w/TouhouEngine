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

private:
  Microsoft::WRL::ComPtr<ID3D11Device> m_device;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
  Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
};
} // namespace Graphics
