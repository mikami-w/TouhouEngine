#include "Core/MathUtils.hpp"

namespace Game::BulletEmitter {

template <SimpleMotion BulletMotion>
BulletHandle SpawnBullet(Core::Vector2 pos, BulletMotion motionModule, BulletMetadata metadata) noexcept
{
  return detail::g_bulletRouter->spawnBullet(pos, motionModule, metadata);
}

inline std::vector<BulletHandle> EmitRing(Core::Vector2 center,
                                          BulletMotionP firstMotionModule,
                                          BulletMetadata metadata,
                                          int count,
                                          float radius)
{
  float const angleStep = Core::Math::PI2_f / count; // 每颗子弹之间的角度间隔
  std::vector<BulletHandle> handles;
  handles.reserve(count);

  for (int i = 0; i < count; ++i) {
    handles.push_back(SpawnBullet(center, firstMotionModule, metadata));
    firstMotionModule.angle += angleStep; // 旋转角度, 形成环形
  }
  return handles;
}
} // namespace Game::BulletEmitter
