#pragma once

#include "BulletManager.hpp"
#include "BulletSystem.hpp"
#include "Core/BasicTypes.hpp"

namespace Game {
// 纯静态方法类
class BulletEmitter
{
public:
  static void emitRing(Core::Position center, float angle, float speed, int count);

  static void init(BulletSystem* bulletSystem) noexcept { m_bulletSystem = bulletSystem; }

  static void spawnBullet(BulletPosition pos, BulletMotionP const& bulletMotion, BulletMetadata meta) noexcept
  {
    m_bulletSystem->spawnBullet(pos, bulletMotion, meta);
  }
  // 使用矢量速度创造子弹
  void spawnBulletP(Core::Vector2 pos, Core::Vector2 vel, Core::Vector2 accel, BulletMetadata meta) noexcept;
  // 使用角(弧度制)和速率创造子弹
  void spawnBulletR(Core::Vector2 pos,
                    float angle,
                    float angVel,
                    float angAccel,
                    float speed,
                    float tanAccel,
                    BulletMetadata meta) noexcept;

private:
  inline static BulletSystem* m_bulletSystem = nullptr; // 不持有资源, 仅观察
};
} // namespace Game
