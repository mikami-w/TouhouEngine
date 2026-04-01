#pragma once

#include "Bullet.hpp"
#include "BulletManager.hpp"

namespace Game::BulletSystem {
namespace detail {
inline BulletManager* g_bulletManager = nullptr; // 不持有资源, 仅观察

inline void Init(BulletManager* bulletManager) noexcept
{
  g_bulletManager = bulletManager;
}
} // namespace detail

void CancelBullet(BulletManager::Handle handle, bool spawnBonus = false) noexcept;

void SetMotionModule(BulletManager::Handle handle, MotionModule newMotion) noexcept;
void SetPosition(BulletManager::Handle handle, Core::Vector2 pos) noexcept;
void SetMetadata(BulletManager::Handle handle, BulletMetadata meta) noexcept;

// TODO: getters

void ClearBullets() noexcept;

} // namespace Game::BulletSystem

#include "BulletSystem.ipp"
