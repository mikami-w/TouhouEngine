#pragma once

#include "DX11Device.hpp"

#include <memory>
#include <string>
#include <wrl/client.h>

namespace Graphics {
class Texture
{
public:
  Texture(DX11Device* device, std::string const& filePath);
  ~Texture() = default;

  Texture(Texture const&) = delete;
  Texture& operator=(Texture const&) = delete;

  ID3D11ShaderResourceView* getSRV() const { return m_srv.Get(); }

  int getWidth() const { return m_width; }
  int getHeight() const { return m_height; }

private:
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
  int m_width;
  int m_height;
};
} // namespace Graphics
