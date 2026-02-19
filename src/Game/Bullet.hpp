#pragma once

#include <cstdint>

namespace Game {
struct Bullet
{
  float x = 0; // 位置
  float y = 0;
  float vx = 0; // 速度
  float vy = 0;
  std::uint16_t type = 0;  // 弹幕(贴图)类型
  std::uint16_t color = 0; // 颜色
};
} // namespace Game
