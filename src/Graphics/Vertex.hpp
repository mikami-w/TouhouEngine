#pragma once

#include "Core/BasicTypes.hpp"

namespace Graphics {
struct Vertex
{
  Core::Vector3 position; // x, y, x
  Core::Vector2 texCoord; // u, v

  Vertex() = default;
  Vertex(float x, float y, float z, float u, float v)
    : position(x, y, z)
    , texCoord(u, v)
  {
  }
};

struct InstanceData
{
  Core::Vector2 position; // 8 bytes
  Core::Vector2 scale;    // 8 bytes
  float rotation;         // 4 bytes
  Core::Vector4 uvRect;   // 16 bytes, 贴图坐标矩形 (u, v, width, height)
  Core::Vector4 color;    // 16 bytes, RGBA 颜色, 每个分量范围 [0, 1]
};
} // namespace Graphics
