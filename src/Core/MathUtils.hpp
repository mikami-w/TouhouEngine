#pragma once

#if defined(min)
#undef min
#endif
#if defined(max)
#undef max
#endif

#include "Core/Logger.hpp"

#include <array>
#include <bit>
#include <cmath>
#include <numbers>

namespace Core::Math {
constexpr float _2PI = std::numbers::pi_v<float> * 2.0f;            // 360 degree
constexpr float _PI_f = std::numbers::pi_v<float>;                  // 180 degree
constexpr float _5PI_6_f = std::numbers::pi_v<float> * 5.0f / 6.0f; // 150 degree
constexpr float _2PI_3_f = std::numbers::pi_v<float> * 2.0f / 3.0f; // 120 degree
constexpr float _PI_2_f = std::numbers::pi_v<float> / 2.0f;         // 90 degree
constexpr float _PI_3_f = std::numbers::pi_v<float> / 3.0f;         // 60 degree
constexpr float _PI_4_f = std::numbers::pi_v<float> / 4.0f;         // 45 degree
constexpr float _PI_6_f = std::numbers::pi_v<float> / 6.0f;         // 30 degree
constexpr float _PI_8_f = std::numbers::pi_v<float> / 8.0f;         // 22.5 degree
constexpr float _PI_12_f = std::numbers::pi_v<float> / 12.0f;       // 15 degree

namespace details {
inline constexpr float RadToIndex = 1024.0f / _PI_f;

// 四分之一圆周的正弦值表, 从 0 到 pi/2, 含 pi/2
alignas(64) inline static std::array<float, 513> const sinTable = []() {
  std::array<float, 513> table{};
  constexpr float step = _2PI / 2048.0f; // 步长为 2PI / 2048
  for (int i = 0; i < 513; ++i) {
    table[i] = std::sin(step * i);
  }
  table[0] = 0.0f; // 确保精确度
  table[512] = 1.0f;
  return table;
}();

// 将 [0, 2048) 范围的索引折叠到 512 范围并处理函数值符号
__forceinline float sinIndexMapping(int fullIndex) noexcept
{
  // 处理半周镜像, 将 [0, 1023] 映射到 [0, 512, 0]
  int diff = (fullIndex & 1023) - 512; // (fullIndex & 1023) 是 [0, 1023] 范围内的半周索引
  int mask = diff >> 31;
  int absDiff = (diff ^ mask) - mask; // 位运算取 abs(diff): (x ^ mask) - mask，其中 mask 为 x >> 31
  int quarterIdx = 512 - absDiff;     // 映射后的索引：512 - abs(index - 512)

  // 查表
  float val = sinTable[quarterIdx];

  // IEEE 754 浮点数第 31 位是符号位
  // 把 fullIndex 的第 10 位（1024）移动到第 31 位。
  std::uint32_t signBit = (static_cast<std::uint32_t>(fullIndex) & 1024) << 21;

  // 将 float 视作 uint32_t 进行异或
  std::uint32_t valInt = std::bit_cast<std::uint32_t>(val) ^ signBit; // 异或符号位：如果是 1 则变负，0 则保持正

  return std::bit_cast<float>(valInt);
}
} // namespace Core::Math::details

// 简单函数/有硬件优化的函数
template <typename T>
__forceinline constexpr T min(T a, T b) noexcept
{
  return std::min(a, b);
}

template <typename T>
__forceinline constexpr T max(T a, T b) noexcept
{
  return std::max(a, b);
}

template <typename T>
__forceinline T abs(T x) noexcept
{
  return std::abs(x);
}

template <typename T>
__forceinline T ceil(T x) noexcept
{
  return std::ceil(x);
}

template <typename T>
__forceinline T floor(T x) noexcept
{
  return std::floor(x);
}

template <typename T>
__forceinline T trunc(T x) noexcept
{
  return std::trunc(x);
}

template <typename T>
__forceinline T round(T x) noexcept
{
  return std::round(x);
}

template <typename T>
__forceinline T fmod(T x, T y) noexcept
{
  return std::fmod(x, y);
}

// 三角/反三角函数
__forceinline float sin(float radians) noexcept
{
  // 将角度转换为 [0, 2048) 范围内的索引
  // 2047 = 0x7FF = 0b111'1111'1111, 处理负数和大于 2pi 的情况
  int index = static_cast<int>(std::floor(radians * details::RadToIndex)) & 2047;
  return details::sinIndexMapping(index);
}

__forceinline float cos(float radians) noexcept
{
  // cos(x) = sin(x + pi/2). 在 2048 长度的表中，pi/2 偏移量是 512
  int index = static_cast<int>(std::floor(radians * details::RadToIndex + 512.0f)) & 2047;
  return details::sinIndexMapping(index);
}

inline void initMathUtils()
{
  LOG_INFO("Initializing MathUtils...");

  // 触发 sinTable 的初始化
  (void)details::sinTable;

  LOG_INFO("MathUtils initialized.");
}
} // namespace Core::Math
