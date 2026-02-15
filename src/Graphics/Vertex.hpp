#pragma once

#include <DirectXMath.h>

namespace Graphics {
struct Vertex
{
  DirectX::XMFLOAT3 position; // x, y, x
  DirectX::XMFLOAT2 texCoord; // u, v

  Vertex() = default;
  Vertex(float x, float y, float z, float u, float v)
    : position(x, y, z)
    , texCoord(u, v)
  {
  }
};
} // namespace Graphics
