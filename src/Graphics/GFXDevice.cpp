#include "GFXDevice.hpp"

#include "Core/Logger.hpp"

namespace Graphics {

GFXDevice::GFXDevice(SDL_Window* window, bool enableVsync)
{
  bool debugMode = false;
#ifdef _DEBUG
  debugMode = true;
#endif

  m_device = makeGPUDevice(
    SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_METALLIB,
    debugMode
  );

  if (!m_device) {
    LOG_ERROR(std::format("SDL_CreateGPUDevice failed: {}", SDL_GetError()));
    throw std::runtime_error("Failed to create GPU device");
  }

  if (!SDL_ClaimWindowForGPUDevice(m_device.get(), window)) {
    LOG_ERROR(std::format("SDL_ClaimWindowForGPUDevice failed: {}", SDL_GetError()));
    throw std::runtime_error("Failed to claim window for GPU");
  }

  SDL_GPUPresentMode presentMode =
    enableVsync ? SDL_GPU_PRESENTMODE_VSYNC : SDL_GPU_PRESENTMODE_IMMEDIATE;
  if (!SDL_WindowSupportsGPUPresentMode(m_device.get(), window, presentMode)) {
    LOG_WARN("Immediate present mode is unsupported. Falling back to vertical sync.");
    presentMode = SDL_GPU_PRESENTMODE_VSYNC;
  }

  if (!SDL_SetGPUSwapchainParameters(
        m_device.get(), window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, presentMode)) {
    LOG_ERROR(std::format("SDL_SetGPUSwapchainParameters failed: {}", SDL_GetError()));
    throw std::runtime_error("Failed to configure GPU swapchain");
  }

  m_swapchainFormat = SDL_GetGPUSwapchainTextureFormat(m_device.get(), window);
  LOG_INFO(std::format("SDL_GPU Device created successfully. Present mode: {}.",
                       presentMode == SDL_GPU_PRESENTMODE_VSYNC ? "VSYNC" : "IMMEDIATE"));
}

GFXDevice::SwapchainInfo GFXDevice::acquireSwapchain(SDL_GPUCommandBuffer* cmd, SDL_Window* window)
{
  SDL_GPUTexture* swapchainTex = nullptr;
  Uint32 w = 0, h = 0;

  if (!SDL_AcquireGPUSwapchainTexture(cmd, window, &swapchainTex, &w, &h)) {
    LOG_ERROR(std::format("SDL_AcquireGPUSwapchainTexture failed: {}", SDL_GetError()));
    return { nullptr, 0, 0 };
  }

  return { swapchainTex, w, h };
}

} // namespace Graphics
