#pragma once

#include <SDL3/SDL_gpu.h>
#include "GPUResourceManager.hpp"

namespace Graphics {

class GFXDevice
{
public:
  GFXDevice(SDL_Window* window, bool enableVsync);
  ~GFXDevice() = default;

  GFXDevice(GFXDevice const&) = delete;
  GFXDevice& operator=(GFXDevice const&) = delete;

  SDL_GPUDevice* getDevice() const { return m_device.get(); }
  SDL_GPUTextureFormat getSwapchainFormat() const { return m_swapchainFormat; }

  // 获取 swapchain texture (每帧调用)
  // 返回 {texture, width, height}, texture 可能为 nullptr (窗口最小化等)
  struct SwapchainInfo
  {
    SDL_GPUTexture* texture;
    Uint32 width;
    Uint32 height;
  };
  SwapchainInfo acquireSwapchain(SDL_GPUCommandBuffer* cmd, SDL_Window* window);

private:
  GPUDevicePtr m_device;
  SDL_GPUTextureFormat m_swapchainFormat;
};

} // namespace Graphics
