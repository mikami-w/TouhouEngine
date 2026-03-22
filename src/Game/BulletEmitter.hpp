#pragma once

#include "Bullet.hpp"
#include "BulletRouter.hpp"
#include "Core/BasicTypes.hpp"

namespace Game::BulletEmitter {
namespace detail {
inline BulletRouter* g_bulletRouter = nullptr; // 不持有资源, 仅观察

inline void init(BulletRouter* bulletRouter) noexcept
{
  g_bulletRouter = bulletRouter;
}

} // namespace detail

// 发射环状子弹
void emitRing(Position center, float angle, float speed, int count, float radius);

// 使用矢量速度创造子弹
void spawnBulletP(Position pos, Core::Vector2 vel, Core::Vector2 accel, BulletMetadata meta) noexcept;

// 使用角(弧度制)和速率创造子弹
void spawnBulletR(Position pos,
                  float angle,
                  float angVel,
                  float angAccel,
                  float speed,
                  float tanAccel,
                  BulletMetadata meta) noexcept;

template <typename BulletMotion>
void spawnBullet(Position pos, BulletMotion const& bulletMotion, BulletMetadata meta) noexcept
{
  detail::g_bulletRouter->spawnBullet(pos, bulletMotion, meta);
}
} // namespace Game::BulletEmitter
