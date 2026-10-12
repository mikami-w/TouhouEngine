#pragma once

#include <SDL3/SDL_gpu.h>
#include <memory>

namespace Graphics {

// 无状态删除器 (用于 SDL_Window 等不需要 device 参数的资源)
template <auto DestroyFn>
struct StatelessDeleter
{
  template <typename T>
  void operator()(T* ptr) const
  {
    if (ptr) {
      DestroyFn(ptr);
    }
  }
};

// 有状态删除器 (用于需要 SDL_GPUDevice* 参数的 GPU 资源)
struct GPUDestroyer
{
  SDL_GPUDevice* device = nullptr;

  void operator()(SDL_GPUShader* s) const
  {
    if (s && device) {
      SDL_ReleaseGPUShader(device, s);
    }
  }
  void operator()(SDL_GPUGraphicsPipeline* p) const
  {
    if (p && device) {
      SDL_ReleaseGPUGraphicsPipeline(device, p);
    }
  }
  void operator()(SDL_GPUBuffer* b) const
  {
    if (b && device) {
      SDL_ReleaseGPUBuffer(device, b);
    }
  }
  void operator()(SDL_GPUTexture* t) const
  {
    if (t && device) {
      SDL_ReleaseGPUTexture(device, t);
    }
  }
  void operator()(SDL_GPUSampler* s) const
  {
    if (s && device) {
      SDL_ReleaseGPUSampler(device, s);
    }
  }
  void operator()(SDL_GPUTransferBuffer* tb) const
  {
    if (tb && device) {
      SDL_ReleaseGPUTransferBuffer(device, tb);
    }
  }
};

// 类型别名
using GPUDevicePtr = std::unique_ptr<SDL_GPUDevice, StatelessDeleter<SDL_DestroyGPUDevice>>;
using ShaderPtr = std::unique_ptr<SDL_GPUShader, GPUDestroyer>;
using PipelinePtr = std::unique_ptr<SDL_GPUGraphicsPipeline, GPUDestroyer>;
using GPUBufferPtr = std::unique_ptr<SDL_GPUBuffer, GPUDestroyer>;
using GPUTexturePtr = std::unique_ptr<SDL_GPUTexture, GPUDestroyer>;
using GPUSamplerPtr = std::unique_ptr<SDL_GPUSampler, GPUDestroyer>;
using TransferBufPtr = std::unique_ptr<SDL_GPUTransferBuffer, GPUDestroyer>;

// 工厂函数
[[nodiscard]] inline GPUDevicePtr makeGPUDevice(SDL_GPUShaderFormat formats, bool debug)
{
  return GPUDevicePtr(SDL_CreateGPUDevice(formats, debug, nullptr));
}

[[nodiscard]] inline ShaderPtr makeShader(SDL_GPUDevice* dev, SDL_GPUShaderCreateInfo const& info)
{
  return ShaderPtr(SDL_CreateGPUShader(dev, &info), GPUDestroyer{ dev });
}

[[nodiscard]] inline PipelinePtr makePipeline(SDL_GPUDevice* dev, SDL_GPUGraphicsPipelineCreateInfo const& info)
{
  return PipelinePtr(SDL_CreateGPUGraphicsPipeline(dev, &info), GPUDestroyer{ dev });
}

[[nodiscard]] inline GPUBufferPtr makeBuffer(SDL_GPUDevice* dev, SDL_GPUBufferCreateInfo const& info)
{
  return GPUBufferPtr(SDL_CreateGPUBuffer(dev, &info), GPUDestroyer{ dev });
}

[[nodiscard]] inline GPUTexturePtr makeTexture(SDL_GPUDevice* dev, SDL_GPUTextureCreateInfo const& info)
{
  return GPUTexturePtr(SDL_CreateGPUTexture(dev, &info), GPUDestroyer{ dev });
}

[[nodiscard]] inline GPUSamplerPtr makeSampler(SDL_GPUDevice* dev, SDL_GPUSamplerCreateInfo const& info)
{
  return GPUSamplerPtr(SDL_CreateGPUSampler(dev, &info), GPUDestroyer{ dev });
}

[[nodiscard]] inline TransferBufPtr makeTransferBuffer(SDL_GPUDevice* dev, SDL_GPUTransferBufferCreateInfo const& info)
{
  return TransferBufPtr(SDL_CreateGPUTransferBuffer(dev, &info), GPUDestroyer{ dev });
}

} // namespace Graphics
