#include "DX11Device.hpp"
#include "Core/Logger.hpp"

namespace Graphics {
DX11Device::DX11Device(HWND hWnd, int width, int height)
{
  // 描述交换链的属性 (描述书)
  DXGI_SWAP_CHAIN_DESC sd{};
  sd.BufferDesc.Width = width;
  sd.BufferDesc.Height = height;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32位色彩 (红绿蓝透明度各8位)
  sd.BufferDesc.RefreshRate.Numerator = 60; // 刷新率分子
  sd.BufferDesc.RefreshRate.Denominator = 1; // 刷新率分母 (60/1 = 60Hz)
  sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
  sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

  // 多重采样抗锯齿 (MSAA), 对于2D弹幕游戏通常关掉以提升性能, 设为 1 次采样
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;

  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 作为输出的缓冲区
  sd.BufferCount = 1; // 后台缓冲区的数量 (1个后台 + 1个隐藏的前台 = 双缓冲)
  sd.OutputWindow = hWnd; // 绑定到窗口
  sd.Windowed = TRUE; // 窗口化模式
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // 翻转后丢弃后台缓冲区内容
  sd.Flags = 0;

  // 开启 Debug 标志 (让 DX11 在控制台打印极度详细的错误信息)
  UINT createDeviceFlags = 0;
#if defined(_DEBUG)
  createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  // 创建设备, 上下文, 交换链
  HRESULT hr = D3D11CreateDeviceAndSwapChain(
    nullptr,  // 默认显卡
    D3D_DRIVER_TYPE_HARDWARE, // 使用硬件(GPU)加速, 不用软件模拟
    nullptr,  // 软件光栅化器指针 (不使用)
    createDeviceFlags,  // 调试标志
    nullptr,  // 特性等级数组 (传入 nullptr 默认选取最高支持版本)
    0,  // 特性等级数组大小
    D3D11_SDK_VERSION,  // SDK 版本 (宏)
    &sd,  // 交换链描述书
    m_swapChain.GetAddressOf(), // [输出] 交换链
    m_device.GetAddressOf(),  // [输出] 设备
    nullptr,  // [输出] 实际获得的特性等级 (不关心, 传 nullptr)
    m_context.GetAddressOf() // [输出] 设备上下文
    );

  LOG_DX11_CHECK(hr, "Failed to create D3D11 Device and SwapChain!");
  LOG_INFO("D3D11 Device, Context, and SwapChain created successfully.");
}
} // namespace Graphics
