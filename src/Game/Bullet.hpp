#pragma once

#include <cstdint>

#include "Core/BasicTypes.hpp"
#include "Core/MathUtils.hpp"
#include "Graphics/Color.hpp"
#include "Graphics/SpriteType.hpp"

namespace Game {

struct BulletMetadata
{
  Graphics::SpriteType type = Graphics::SpriteType::ERROR_404; // 子弹 (贴图) 类型
  Graphics::Color::ColorType color = 0;                        // RGBA 颜色
};

enum class BulletMotionType : std::uint8_t
{
  Polar,
  Rectangular
};

template <typename T>
concept SimpleMotion = requires(T motion, Core::Vector2& pos) {
  // 要求必须存在 updatePosition 成员函数，且接受 Position& 参数
  // 并且返回类型必须是 void (或者是能转换成 void 的，通常用 std::same_as<void> 约束)
  { motion.updatePosition(pos) } -> std::same_as<void>;
};

// BulletMotion in Polar coordinates, 极坐标系中运动的子弹
struct BulletMotionP
{
  float angle = 0;    // 角 (弧度制), 与 +x 方向 (水平向右) 的顺时针夹角
  float angVel = 0;   // 角速度, 每帧旋转的弧度
  float angAccel = 0; // 角加速度, 每帧增加的角速度 (弧度)
  float speed = 0;    // 速率, 每帧移动的像素数
  float tanAccel = 0; // 切向加速度, 每帧增加的速率

  __forceinline void updatePosition(Core::Vector2& pos) noexcept
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

  __forceinline void updatePosition(Core::Vector2& pos) noexcept
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
  Core::Vector2* position = nullptr;
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
