#pragma once

#include <cmath>

namespace Core {

struct Vector2
{
  float x = 0.0f, y = 0.0f;
  Vector2() = default;
  constexpr Vector2(float x, float y) : x(x), y(y) {}
};

struct Vector3
{
  float x = 0.0f, y = 0.0f, z = 0.0f;
  Vector3() = default;
  constexpr Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct Vector4
{
  float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;
  Vector4() = default;
  constexpr Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
};

} // namespace Core
