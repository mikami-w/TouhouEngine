#include "BulletEmitter.hpp"

#include "Core/MathUtils.hpp"

namespace Game {

void BulletEmitter::emitRing(Position center, float angle, float speed, int count, float radius)
{
  float const angleStep = Core::Math::PI2_f / count; // 每颗子弹之间的角度间隔

  BulletMotionP motion{ .angle = angle, .speed = speed };
  for (int i = 0; i < count; ++i) {
    spawnBullet(center, motion, {});
    motion.angle += angleStep; // 旋转角度, 形成环形
  }
}

void BulletEmitter::spawnBulletP(Position pos, Core::Vector2 vel, Core::Vector2 accel, BulletMetadata meta) noexcept
{
  detail::g_bulletRouter->spawnBullet(pos, BulletMotionR{ .vel = vel, .accel = accel }, meta);
}

void BulletEmitter::spawnBulletR(Position pos,
                                 float angle,
                                 float angVel,
                                 float angAccel,
                                 float speed,
                                 float tanAccel,
                                 BulletMetadata meta) noexcept
{
  detail::g_bulletRouter->spawnBullet(
    pos,
    BulletMotionP{ .angle = angle, .angVel = angVel, .angAccel = angAccel, .speed = speed, .tanAccel = tanAccel },
    meta);
}
} // namespace Game
