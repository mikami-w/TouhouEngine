#include "Core/MathUtils.hpp"

namespace Game::BulletEmitter {

inline BulletManager::Handle SpawnBullet(Core::Vector2 pos, MotionModule motionModule, BulletMetadata metadata) noexcept
{
  return detail::g_bulletManager->spawnBullet(pos, motionModule, metadata);
}

inline void EmitRing(Core::Vector2 center,
                     MotionModule firstMotionModule,
                     BulletMetadata metadata,
                     int count,
                     float radius)
{
  float const angleStep = Core::Math::PI2_f / count; // 每颗子弹之间的角度间隔
  auto motion = firstMotionModule;
  motion.polar.angle += Core::Math::PI_2_f;

  for (int i = 0; i < count; ++i) {
    Core::Vector2 offset{ center.x + radius * Core::Math::cos(firstMotionModule.polar.angle),
                          center.y + radius * Core::Math::sin(firstMotionModule.polar.angle) };
    SpawnBullet(offset, motion, metadata); // 原样保留除 angle 之外的其他运动参数
    motion.polar.angle += angleStep;       // 旋转角度, 形成环形
    firstMotionModule.polar.angle += angleStep; // 旋转发射模组的初始角度, 形成旋转环
  }
}

inline void EmitFan(Core::Vector2 center,
                    MotionModule firstMotionModule,
                    BulletMetadata metadata,
                    int count,
                    float radius)
{
  float const angleStep = Core::Math::PI2_f / count; // 每颗子弹之间的角度间隔

  for (int i = 0; i < count; ++i) {
    Core::Vector2 offset{ center.x + radius * Core::Math::cos(firstMotionModule.polar.angle),
                          center.y + radius * Core::Math::sin(firstMotionModule.polar.angle) };
    SpawnBullet(offset, firstMotionModule, metadata); // 原样保留除 angle 之外的其他运动参数
    firstMotionModule.polar.angle += angleStep; // 旋转发射模组的初始角度, 形成旋转环
  }
}
} // namespace Game::BulletEmitter
