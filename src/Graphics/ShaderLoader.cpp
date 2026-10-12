#include "ShaderLoader.hpp"

#include <fstream>

#include "Core/Logger.hpp"

namespace Graphics {

ShaderPtr ShaderLoader::load(SDL_GPUDevice* device,
                             std::string const& basePath,
                             std::string const& shaderName,
                             SDL_GPUShaderStage stage,
                             ShaderResourceInfo const& resources)
{
  SDL_GPUShaderFormat supportedFormats = SDL_GetGPUShaderFormats(device);

  // 按优先级尝试加载: SPIR-V > DXIL > Metallib
  struct FormatAttempt
  {
    SDL_GPUShaderFormat format;
    char const* extension;
  };

  FormatAttempt formats[] = {
    { SDL_GPU_SHADERFORMAT_SPIRV, ".spv" },
    { SDL_GPU_SHADERFORMAT_DXIL, ".dxil" },
    { SDL_GPU_SHADERFORMAT_METALLIB, ".metallib" },
  };

  for (auto const& [format, ext] : formats) {
    if (supportedFormats & format) {
      std::string path = basePath + "/" + shaderName + ext;
      auto shader = tryLoad(device, path, format, stage, resources);
      if (shader) {
        LOG_INFO(std::format("Loaded shader: {} ({})", path, ext));
        return shader;
      }
    }
  }

  LOG_ERROR(std::format("Failed to load shader '{}' in any supported format.", shaderName));
  throw std::runtime_error("Failed to load shader: " + shaderName);
}

std::vector<Uint8> ShaderLoader::readFile(std::string const& path)
{
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    return {};
  }

  auto size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<Uint8> buffer(static_cast<size_t>(size));
  file.read(reinterpret_cast<char*>(buffer.data()), size);
  return buffer;
}

ShaderPtr ShaderLoader::tryLoad(SDL_GPUDevice* device,
                                std::string const& path,
                                SDL_GPUShaderFormat format,
                                SDL_GPUShaderStage stage,
                                ShaderResourceInfo const& resources)
{
  auto code = readFile(path);
  if (code.empty()) {
    return nullptr;
  }

  SDL_GPUShaderCreateInfo createInfo{};
  createInfo.code_size = code.size();
  createInfo.code = code.data();
  createInfo.entrypoint = "main";
  createInfo.format = format;
  createInfo.stage = stage;
  createInfo.num_samplers = resources.numSamplers;
  createInfo.num_storage_textures = resources.numStorageTextures;
  createInfo.num_storage_buffers = resources.numStorageBuffers;
  createInfo.num_uniform_buffers = resources.numUniformBuffers;
  createInfo.props = 0;

  return makeShader(device, createInfo);
}

} // namespace Graphics
