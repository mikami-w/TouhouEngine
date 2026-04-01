#pragma once

#include "Bullet.hpp"
#include "BulletManager.hpp"
#include "Core/BasicTypes.hpp"

namespace Game::BulletEmitter {
namespace detail {
inline BulletManager* g_bulletManager = nullptr; // 不持有资源, 仅观察

inline void Init(BulletManager* bulletManager) noexcept
{
  g_bulletManager = bulletManager;
}
} // namespace detail

BulletManager::Handle SpawnBullet(Core::Vector2 pos, MotionModule motionModule, BulletMetadata metadata) noexcept;

void EmitRing(Core::Vector2 center, MotionModule firstMotionModule, BulletMetadata metadata, int count, float radius);

void EmitFan(Core::Vector2 center, MotionModule firstMotionModule, BulletMetadata metadata, int count, float radius);
// 更多发射方式待拓展
} // namespace Game::BulletEmitter

#include "BulletEmitter.ipp"
