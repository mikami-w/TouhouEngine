#pragma once

#include "Bullet.hpp"
#include "BulletRouter.hpp"

namespace Game::BulletSystem {
namespace detail {
inline BulletRouter* g_bulletRouter = nullptr; // 不持有资源, 仅观察

inline void init(BulletRouter* bulletRouter) noexcept
{
  g_bulletRouter = bulletRouter;
}
} // namespace detail

BulletView getBullet(BulletHandle handle);

void cancelBullet(BulletHandle handle, bool spawnBonus = false) noexcept;

template <SimpleMotion BulletMotion>
void setMotionModule(BulletHandle& handle, BulletMotion newMotion) noexcept;

void setPosition(BulletHandle handle, Position pos) noexcept;

void setMetadata(BulletHandle handle, BulletMetadata meta) noexcept;

void clearBullets() noexcept;

} // namespace Game::BulletSystem

#include "BulletSystem.ipp"