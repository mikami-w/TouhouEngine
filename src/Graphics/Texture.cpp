#include "Texture.hpp"

#include <stb/stb_image.h>

#include "Core/Logger.hpp"

namespace Graphics {

Texture::Texture(SDL_GPUDevice* device, SDL_GPUSampler* sampler, std::string const& filePath)
  : m_sampler(sampler)
{
  LOG_INFO("Loading texture: " + filePath);

  int channels;

  // CPU 解码
  auto pixels = stbi_load(filePath.c_str(), &m_width, &m_height, &channels, STBI_rgb_alpha);
  if (!pixels) {
    LOG_ERROR("Failed to load image: " + filePath);
    throw std::runtime_error("Failed to load image.");
  }

  // 创建 GPU 纹理
  SDL_GPUTextureCreateInfo texInfo{};
  texInfo.type = SDL_GPU_TEXTURETYPE_2D;
  texInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
  texInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
  texInfo.width = static_cast<Uint32>(m_width);
  texInfo.height = static_cast<Uint32>(m_height);
  texInfo.layer_count_or_depth = 1;
  texInfo.num_levels = 1;
  texInfo.sample_count = SDL_GPU_SAMPLECOUNT_1;

  m_texture = makeTexture(device, texInfo);
  if (!m_texture) {
    stbi_image_free(pixels);
    LOG_ERROR(std::format("Failed to create GPU texture: {}", SDL_GetError()));
    throw std::runtime_error("Failed to create GPU texture.");
  }

  // 创建 Transfer Buffer 用于上传
  Uint32 dataSize = static_cast<Uint32>(m_width * m_height * 4);
  SDL_GPUTransferBufferCreateInfo transferInfo{};
  transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
  transferInfo.size = dataSize;

  TransferBufPtr transferBuf = makeTransferBuffer(device, transferInfo);
  if (!transferBuf) {
    stbi_image_free(pixels);
    LOG_ERROR(std::format("Failed to create transfer buffer: {}", SDL_GetError()));
    throw std::runtime_error("Failed to create transfer buffer.");
  }

  // Map → memcpy → Unmap
  auto* mapped = static_cast<Uint8*>(SDL_MapGPUTransferBuffer(device, transferBuf.get(), false));
  if (!mapped) {
    stbi_image_free(pixels);
    LOG_ERROR(std::format("Failed to map transfer buffer: {}", SDL_GetError()));
    throw std::runtime_error("Failed to map transfer buffer.");
  }
  memcpy(mapped, pixels, dataSize);
  SDL_UnmapGPUTransferBuffer(device, transferBuf.get());
  stbi_image_free(pixels);

  // Copy Pass: 上传到 GPU 纹理
  SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
  if (!cmd) {
    LOG_ERROR(std::format("Failed to acquire command buffer: {}", SDL_GetError()));
    throw std::runtime_error("Failed to acquire command buffer.");
  }

  SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);

  SDL_GPUTextureTransferInfo transferSrc{};
  transferSrc.transfer_buffer = transferBuf.get();
  transferSrc.offset = 0;
  transferSrc.pixels_per_row = static_cast<Uint32>(m_width);
  transferSrc.rows_per_layer = static_cast<Uint32>(m_height);

  SDL_GPUTextureRegion transferDst{};
  transferDst.texture = m_texture.get();
  transferDst.mip_level = 0;
  transferDst.layer = 0;
  transferDst.x = 0;
  transferDst.y = 0;
  transferDst.z = 0;
  transferDst.w = static_cast<Uint32>(m_width);
  transferDst.h = static_cast<Uint32>(m_height);
  transferDst.d = 1;

  SDL_UploadToGPUTexture(copyPass, &transferSrc, &transferDst, false);

  SDL_EndGPUCopyPass(copyPass);
  SDL_SubmitGPUCommandBuffer(cmd);

  LOG_INFO("Texture loaded successfully: " + filePath);
}

SDL_GPUTextureSamplerBinding Texture::getBinding() const
{
  SDL_GPUTextureSamplerBinding binding{};
  binding.texture = m_texture.get();
  binding.sampler = m_sampler;
  return binding;
}

} // namespace Graphics
