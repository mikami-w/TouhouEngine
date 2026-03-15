#pragma once

#include <cstdint>

#include "Core/BasicTypes.hpp"
#include "Core/MathUtils.hpp"

namespace Game {
struct BulletMetadata
{
  std::uint16_t type = 0;  // 子弹 (贴图) 类型
  std::uint16_t color = 0; // 颜色
};

struct Bullet
{
  Core::Position pos{};  // 屏幕坐标位置
  float angle = 0;       // 角 (弧度制), 与 +x 方向 (水平向右) 的顺时针夹角
  float angVel = 0;      // 角速度, 每帧旋转的弧度
  float angAccel = 0;    // 角加速度, 每帧增加的角速度 (弧度)
  float speed = 0;       // 速率, 每帧移动的像素数
  float tanAccel = 0;    // 切向加速度, 每帧增加的速率
  BulletMetadata meta{}; // 其他数据 (类型/颜色等)

  __forceinline void updatePosition() noexcept
  {
    // 半隐式欧拉积分: 先更新速度, 再更新位置
    angVel += angAccel;
    speed += tanAccel;
    angle += angVel;
    pos.x += speed * Core::Math::cos(angle);
    pos.y += speed * Core::Math::sin(angle);
  }
};
} // namespace Game
