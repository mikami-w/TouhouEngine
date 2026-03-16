#include "BulletEmitter.hpp"

#include "Core/MathUtils.hpp"

namespace Game {

void BulletEmitter::emitRing(Core::Position center, float angle, float speed, int count)
{
  float const angleStep = Core::Math::PI2_f / count; // 每颗子弹之间的角度间隔

  BulletMotionP motion{ .angle = angle, .speed = speed };
  for (int i = 0; i < count; ++i) {
    spawnBullet(center, motion, {});
    motion.angle += angleStep; // 旋转角度, 形成环形
  }
}
} // namespace Game
