#pragma once

#include <cstdint>

class XSRandom
{
public:
  // 初始化种子. Xorshift 的种子不为 0
  explicit XSRandom(std::uint32_t seed) { state = seed | (seed == 0); }

  // 设置/重置种子
  void setSeed(std::uint32_t seed) { state = seed | (seed == 0); }

  // 获取当前状态
  std::uint32_t getState() const { return state; }

  // 核心算法：生成下一个随机整数
  std::uint32_t nextInt()
  {
    std::uint32_t x = state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    state = x;
    return x;
  }

  // 生成 [0.0, 1.0) 范围的浮点数: 取低 24 位除以 2^24
  float nextFloat01() { return (nextInt() & 0xFFFFFF) / 16777216.0f; }

  // 生成 [min, max] 范围的浮点数
  float rangedFloat(float min, float max) { return min + nextFloat01() * (max - min); }

private:
  std::uint32_t state;
};
