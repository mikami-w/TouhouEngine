#pragma once

#include <cstdint>

#include "Core/BasicTypes.hpp"
#include "Core/MathUtils.hpp"

namespace Game {
using Position = Core::Position; // 子弹位置, 以屏幕坐标 (像素) 表示

struct BulletMetadata
{
  std::uint8_t type = 0;  // 子弹 (贴图) 类型
  std::uint8_t color = 0; // 颜色
};

enum class BulletMotionType : std::uint8_t
{
  Polar,
  Rectangular
};

// BulletMotion in Polar coordinates, 极坐标系中运动的子弹
struct BulletMotionP
{
  float angle = 0;    // 角 (弧度制), 与 +x 方向 (水平向右) 的顺时针夹角
  float angVel = 0;   // 角速度, 每帧旋转的弧度
  float angAccel = 0; // 角加速度, 每帧增加的角速度 (弧度)
  float speed = 0;    // 速率, 每帧移动的像素数
  float tanAccel = 0; // 切向加速度, 每帧增加的速率

  __forceinline void updatePosition(Position& pos) noexcept
  {
    // 半隐式欧拉积分: 先更新速度, 再更新位置
    angVel += angAccel;
    speed += tanAccel;
    angle += angVel;
    pos.x += speed * Core::Math::cos(angle);
    pos.y += speed * Core::Math::sin(angle);
  }
};

// BulletMotion in Rectangle coordinates, 笛卡尔坐标系 (直角坐标系) 中运动的子弹
struct BulletMotionR
{
  Core::Vector2 vel{};
  Core::Vector2 accel{};

  __forceinline void updatePosition(Position& pos) noexcept
  {
    vel.x += accel.x;
    vel.y += accel.y;
    pos.x += vel.x;
    pos.y += vel.y;
  }
};

struct BulletView
{
  BulletMotionType motionType = BulletMotionType::Polar;
  Position* position = nullptr;
  BulletMetadata* metadata = nullptr;
  union
  {
    void* raw = nullptr;
    BulletMotionP* polar;
    BulletMotionR* rectangular;
  } motion;

  bool isValid() const noexcept { return position != nullptr; }
  operator bool() const noexcept { return isValid(); }
};
} // namespace Game
