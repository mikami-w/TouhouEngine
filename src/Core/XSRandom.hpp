#pragma once

#include <cstdint>

namespace Core::Math {
class XSRandom
{
public:
  // 初始化种子. Xorshift 的种子不为 0
  explicit XSRandom(std::uint32_t seed) { state = seed | (seed == 0); }

  // 设置/重置种子
  void setSeed(std::uint32_t seed) { state = seed | (seed == 0); }

  // 获取当前状态
  std::uint32_t getState() const { return state; }

  // 生成下一个随机整数
  std::uint32_t nextInt()
  {
    std::uint32_t x = state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    state = x;
    return x;
  }

  // 生成 [0.0, 1.0) 范围的浮点数
  float nextFloat01()
  {
    // 构造 IEEE 754 格式: 指数位设为 127 (代表 2^0),
    // 尾数部分填入 23 位随机位
    std::uint32_t bits = (nextInt() >> 9) | 0x3f800000u; // 0x3F800000 是 1.0f 的位表示
    float f = std::bit_cast<float>(bits);
    return f - 1.0f;
  }

  // 生成 [min, max] 范围的浮点数
  float rangedFloat(float min, float max) { return min + nextFloat01() * (max - min); }

  // 以 pTrue 的概率返回 true (0.0 <= pTrue <= 1.0)
  bool nextBoolean(float pTrue = 0.5f)
  {
    if (pTrue <= 0.0f) {
      return false;
    }
    if (pTrue >= 1.0f) {
      return true;
    }

    // 使用 24 位精度进行整数比较, 避开浮点除法
    std::uint32_t threshold = static_cast<std::uint32_t>(pTrue * 16777216.0f); // 16777216 == 2^24
    return (nextInt() & 0xFFFFFF) < threshold;
  }

private:
  std::uint32_t state;
};
} // namespace Core::Math
