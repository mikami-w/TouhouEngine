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

struct InstanceData
{
  DirectX::XMFLOAT2 position; // 8 bytes
  DirectX::XMFLOAT2 scale;    // 8 bytes
  float rotation;             // 4 bytes
  DirectX::XMFLOAT4 color;    // 16 bytes, RGBA 颜色, 每个分量范围 [0, 1]
};
} // namespace Graphics
