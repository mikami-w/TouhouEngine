#pragma once

#include "Core/MathUtils.hpp"

#include <cstdint>

namespace Game {
struct Bullet
{
  float x = 0;             // 屏幕坐标位置
  float y = 0;             // 屏幕坐标位置
  float angle = 0;         // 角 (弧度制), 与 +x 方向 (竖直向下) 的顺时针夹角
  float angVel = 0;        // 角速度, 每帧旋转的弧度
  float angAccel = 0;      // 角加速度, 每帧增加的角速度 (弧度)
  float speed = 0;         // 速率, 每帧移动的像素数
  float tanAccel = 0;      // 切向加速度, 每帧增加的速率
  std::uint16_t type = 0;  // 子弹 (贴图) 类型
  std::uint16_t color = 0; // 颜色

  __forceinline void updatePosition() noexcept
  {
    // 半隐式欧拉积分: 先更新速度, 再更新位置
    angVel += angAccel;
    speed += tanAccel;
    angle += angVel;
    x += speed * Core::Math::cos(angle);
    y += speed * Core::Math::sin(angle);
  }
};
} // namespace Game
