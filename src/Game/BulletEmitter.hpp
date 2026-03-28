#pragma once

#include "Bullet.hpp"
#include "BulletRouter.hpp"
#include "Core/BasicTypes.hpp"

namespace Game::BulletEmitter {
namespace detail {
inline BulletRouter* g_bulletRouter = nullptr; // 不持有资源, 仅观察

inline void Init(BulletRouter* bulletRouter) noexcept
{
  g_bulletRouter = bulletRouter;
}
} // namespace detail


template <SimpleMotion BulletMotion>
BulletHandle SpawnBullet(Position pos, BulletMotion motionModule, BulletMetadata metadata) noexcept;

std::vector<BulletHandle> EmitRing(Position center, BulletMotionP firstMotionModule, int count, float radius);

// 更多发射方式待拓展
} // namespace Game::BulletEmitter

#include "BulletEmitter.ipp"