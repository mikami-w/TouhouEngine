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


template <SimpleMotion BulletMotion>
BulletHandle spawnBullet(Position pos, BulletMotion motionModule, BulletMetadata metadata) noexcept;

void emitRing(Position center, BulletMotionP firstMotionModule, int count, float radius);

} // namespace Game::BulletEmitter

#include "BulletEmitter.ipp"