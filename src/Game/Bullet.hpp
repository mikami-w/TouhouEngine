#pragma once

#include <cstdint>
#include <vector>

#include "Core/BasicTypes.hpp"
#include "Core/MathUtils.hpp"
#include "Graphics/Color.hpp"
#include "Graphics/SpriteType.hpp"

namespace Game {

enum class RotationType : std::uint8_t
{
  None,             // 不旋转
  FaceVelocity,     // 自动脸朝向速度方向
  Fixed,            // 固定角度
  ConstantSpin      // 以恒定角速度自转
};

struct BulletMetadata
{
  Graphics::Color::ColorType color = 0;                        // RGBA 颜色
  Graphics::SpriteType type = Graphics::SpriteType::ERROR_404; // 子弹 (贴图) 类型

};

// 运动模组, 两种运动可叠加
struct MotionModule
{
  // Motion in Polar coordinates, 极坐标系运动模组
  struct Motion_Polar
  {
    float angle = 0.0f;    // 角 (弧度制), 与 +x 方向 (水平向右) 的顺时针夹角
    float angVel = 0.0f;   // 角速度, 每帧旋转的弧度
    float angAccel = 0.0f; // 角加速度, 每帧增加的角速度 (弧度)
    float speed = 0.0f;    // 速率, 每帧移动的像素数
    float tanAccel = 0.0f; // 切向加速度, 每帧增加的速率
  } polar{};

  // Motion in Rectangular coordinates, 笛卡尔坐标系 (直角坐标系) 运动模组
  struct Motion_Rectangular
  {
    float vel_x = 0.0f;   // x 方向速度
    float vel_y = 0.0f;   // y 方向速度
    float accel_x = 0.0f; // x 方向加速度
    float accel_y = 0.0f; // y 方向加速度
  } rect{};
};

struct PositionArray
{
  std::vector<float> x;
  std::vector<float> y;

  PositionArray(std::int32_t capacity) noexcept
    : x(capacity, 0.0f)
    , y(capacity, 0.0f)
  {
  }

  FORCEINLINE void moveTo(std::uint32_t srcIdx, std::uint32_t desIdx) noexcept
  {
    x[desIdx] = x[srcIdx];
    y[desIdx] = y[srcIdx];
  }

  void set(std::uint32_t targetIdx, Core::Vector2 pos) noexcept
  {
    x[targetIdx] = pos.x;
    y[targetIdx] = pos.y;
  }

  void resize(std::size_t newCapacity) noexcept
  {
    x.resize(newCapacity);
    y.resize(newCapacity);
  }
};

// Motion in Polar coordinates, 极坐标系运动模组
struct MotionArray_Polar
{
  std::vector<float> angle;    // 角 (弧度制), 与 +x 方向 (水平向右) 的顺时针夹角
  std::vector<float> angVel;   // 角速度, 每帧旋转的弧度
  std::vector<float> angAccel; // 角加速度, 每帧增加的角速度 (弧度)
  std::vector<float> speed;    // 速率, 每帧移动的像素数
  std::vector<float> tanAccel; // 切向加速度, 每帧增加的速率

  MotionArray_Polar(std::uint32_t capacity) noexcept
    : angle(capacity, 0.0f)
    , angVel(capacity, 0.0f)
    , angAccel(capacity, 0.0f)
    , speed(capacity, 0.0f)
    , tanAccel(capacity, 0.0f)
  {
  }

  FORCEINLINE void moveTo(std::uint32_t srcIdx, std::uint32_t desIdx) noexcept
  {
    angle[desIdx] = angle[srcIdx];
    angVel[desIdx] = angVel[srcIdx];
    angAccel[desIdx] = angAccel[srcIdx];
    speed[desIdx] = speed[srcIdx];
    tanAccel[desIdx] = tanAccel[srcIdx];
  }

  void set(std::uint32_t targetIdx, MotionModule::Motion_Polar const& motion) noexcept
  {
    angle[targetIdx] = motion.angle;
    angVel[targetIdx] = motion.angVel;
    angAccel[targetIdx] = motion.angAccel;
    speed[targetIdx] = motion.speed;
    tanAccel[targetIdx] = motion.tanAccel;
  }

  void resize(std::size_t newCapacity) noexcept
  {
    angle.resize(newCapacity);
    angVel.resize(newCapacity);
    angAccel.resize(newCapacity);
    speed.resize(newCapacity);
    tanAccel.resize(newCapacity);
  }
};

// Motion in Rectangular coordinates, 笛卡尔坐标系 (直角坐标系) 运动模组
struct MotionArray_Rectangular
{
  std::vector<float> vel_x;
  std::vector<float> vel_y;
  std::vector<float> accel_x;
  std::vector<float> accel_y;

  MotionArray_Rectangular(std::uint32_t capacity) noexcept
    : vel_x(capacity, 0.0f)
    , vel_y(capacity, 0.0f)
    , accel_x(capacity, 0.0f)
    , accel_y(capacity, 0.0f)
  {
  }

  FORCEINLINE void moveTo(std::uint32_t src, std::uint32_t des) noexcept
  {
    vel_x[des] = vel_x[src];
    vel_y[des] = vel_y[src];
    accel_x[des] = accel_x[src];
    accel_y[des] = accel_y[src];
  }

  void set(std::uint32_t targetIdx, MotionModule::Motion_Rectangular const& motion) noexcept
  {
    vel_x[targetIdx] = motion.vel_x;
    vel_y[targetIdx] = motion.vel_y;
    accel_x[targetIdx] = motion.accel_x;
    accel_y[targetIdx] = motion.accel_y;
  }

  void resize(std::size_t newCapacity) noexcept
  {
    vel_x.resize(newCapacity);
    vel_y.resize(newCapacity);
    accel_x.resize(newCapacity);
    accel_y.resize(newCapacity);
  }
};

} // namespace Game
