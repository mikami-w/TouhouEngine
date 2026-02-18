#include "Texture.hpp"

#include "Core/Logger.hpp"
#include "stb/stb_image.h"

namespace Graphics {

Texture::Texture(DX11Device* device, std::string const& filePath)
{
  LOG_INFO("Loading texture: " + filePath);

  int channels;

  // CPU 解码
  auto pixels = stbi_load(filePath.c_str(), &m_width, &m_height, &channels, STBI_rgb_alpha);
  if (!pixels) {
    LOG_ERROR("Failed to load image: " + filePath);
    throw std::runtime_error("Failed to load image.");
  }

  // 描述显存特征
  D3D11_TEXTURE2D_DESC texDesc{};
  texDesc.Width = m_width;
  texDesc.Height = m_height;
  texDesc.MipLevels = 1;
  texDesc.ArraySize = 1;
  texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  texDesc.SampleDesc.Count = 1;
  texDesc.Usage = D3D11_USAGE_IMMUTABLE;          // 贴图加载后不会再修改
  texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // 给着色器当资源读取

  // 把 CPU 的像素数组打包
  D3D11_SUBRESOURCE_DATA initData = {};
  initData.pSysMem = pixels;
  initData.SysMemPitch = m_width * 4; // 一行像素占多少字节 (宽度 * 4字节)

  // 创建 2D 纹理对象
  Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
  HRESULT hr = device->getDevice()->CreateTexture2D(&texDesc, &initData, texture.GetAddressOf());
  stbi_image_free(pixels); // 把 CPU 里的像素数组释放掉, 因为数据已经传到显存里了. 一定先释放再 check, 否则可能内存泄漏!
  LOG_DX11_CHECK(hr, "Failed to create Texture2D from image data.");

  // 为这块纹理创建 SRV, 让着色器能认出它
  hr = device->getDevice()->CreateShaderResourceView(texture.Get(), nullptr, m_srv.GetAddressOf());
  LOG_DX11_CHECK(hr, "Failed to create Shader Resource View for Texture.");

  LOG_INFO("Texture loaded successfully.");
}
} // namespace Graphics
