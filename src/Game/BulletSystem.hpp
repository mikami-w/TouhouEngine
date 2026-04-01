#pragma once

#include "Bullet.hpp"
#include "BulletRouter.hpp"

namespace Game::BulletSystem {
namespace detail {
inline BulletRouter* g_bulletRouter = nullptr; // 不持有资源, 仅观察

inline void Init(BulletRouter* bulletRouter) noexcept
{
  g_bulletRouter = bulletRouter;
}
} // namespace detail

// TODO: 不再暴露BulletView, 不让它活到下一条指令, 防止野指针的误用
BulletView GetBullet(BulletHandle handle);

void CancelBullet(BulletHandle handle, bool spawnBonus = false) noexcept;

template <SimpleMotion BulletMotion>
void SetMotionModule(BulletHandle& handle, BulletMotion newMotion) noexcept;

void SetPosition(BulletHandle handle, Core::Vector2 pos) noexcept;

void SetMetadata(BulletHandle handle, BulletMetadata meta) noexcept;

void ClearBullets() noexcept;

} // namespace Game::BulletSystem

#include "BulletSystem.ipp"
