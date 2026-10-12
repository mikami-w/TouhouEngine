#pragma once

#include <SDL3/SDL_gpu.h>
#include <string>
#include <vector>

#include "GPUResourceManager.hpp"

namespace Graphics {

// 着色器资源描述 (编译时已知, 与 HLSL 中的 register 绑定对应)
struct ShaderResourceInfo
{
  Uint32 numSamplers = 0;
  Uint32 numStorageTextures = 0;
  Uint32 numStorageBuffers = 0;
  Uint32 numUniformBuffers = 0;
};

class ShaderLoader
{
public:
  // 根据设备支持的格式, 自动选择并加载预编译着色器
  [[nodiscard]] static ShaderPtr load(SDL_GPUDevice* device,
                                      std::string const& basePath,
                                      std::string const& shaderName,
                                      SDL_GPUShaderStage stage,
                                      ShaderResourceInfo const& resources);

private:
  // 尝试加载特定格式的文件
  [[nodiscard]] static std::vector<Uint8> readFile(std::string const& path);
  [[nodiscard]] static ShaderPtr tryLoad(SDL_GPUDevice* device,
                                          std::string const& path,
                                          SDL_GPUShaderFormat format,
                                          SDL_GPUShaderStage stage,
                                          ShaderResourceInfo const& resources);
};

} // namespace Graphics
