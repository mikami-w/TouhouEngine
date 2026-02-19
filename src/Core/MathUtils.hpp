#pragma once

#include "Core/Logger.hpp"

#include <array>
#include <cmath>
#include <numbers>

namespace Core::Math {
inline constexpr float RadToIndex = 1024.0f / (std::numbers::pi_v<float> * 2.0f);

alignas(64) inline static std::array<float, 1024> const sinTable = []() {
  std::array<float, 1024> table{};
  constexpr float step = std::numbers::pi_v<float> * 2.0f / table.size(); // 每个表项代表的角度步长
  for (int i = 0; i < table.size(); ++i) {
    table[i] = std::sin(step * i); // std::sin 不是 constexpr 函数, 只能在运行时初始化
  }

  constexpr int ii = table.size() / 4; // Integral Index, sin 值为整数的索引间隔

  table[ii * 0] = 0.0f;  // sin(0) = 0
  table[ii * 1] = 1.0f;  // sin(pi/2) = 1
  table[ii * 2] = 0.0f;  // sin(pi) = 0
  table[ii * 3] = -1.0f; // sin(3pi/2) = -1

  return table;
}();

__forceinline inline float sin(float radians) noexcept
{
  // 将角度转换为 [0, 1024) 范围内的索引
  int index = static_cast<int>(radians * RadToIndex) & 1023; // 1023 = 0x3FF = 0b11'1111'1111, 处理负数和大于 2pi 的情况
  return sinTable[index];
};

__forceinline inline float cos(float radians) noexcept
{
  // cos(x) = sin(x + pi/2)。在 1024 长度的表中，pi/2 偏移量是 256
  int index = (static_cast<int>(radians * RadToIndex) + 256) & 1023;
  return sinTable[index];
}

__forceinline inline float sinLerp(float radians) noexcept
{
  float floatIndex = radians * RadToIndex;
  int i = static_cast<int>(std::floor(floatIndex));
  float fraction = floatIndex - static_cast<float>(i); // 计算小数部分

  int idx1 = i & 1023;
  int idx2 = (i + 1) & 1023;

  // 线性插值：a + f * (b - a)
  return sinTable[idx1] + fraction * (sinTable[idx2] - sinTable[idx1]);
}

__forceinline inline float cosLerp(float radians) noexcept
{
  return sinLerp(radians + std::numbers::pi_v<float> / 2.0f);
}

inline void initMathUtils()
{
  LOG_INFO("Initializing MathUtils...");

  // 触发 sinTable 的初始化
  (void)sinTable;

  LOG_INFO("MathUtils initialized.");
}
} // namespace Core::Math
