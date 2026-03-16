#pragma once

#include "Bullet.hpp"
#include "BulletManager.hpp"

namespace Graphics {
class SpriteRenderer;
class TextureManager;
}

namespace Game {
class BulletSystem
{
public:
  BulletSystem() = default;
  ~BulletSystem() = default;

  template <typename BulletMotion>
  void spawnBullet(BulletPosition pos, BulletMotion motion, BulletMetadata meta);

  void init(std::size_t capacity);
  void update(float screenWidth, float screenHeight);
  void clearBullets();

  void draw(Graphics::SpriteRenderer* sr, Graphics::TextureManager* tm);

private:
  BulletManager<BulletMotionP> m_bulletManagerP;
  BulletManager<BulletMotionR> m_bulletManagerR;
};

inline void BulletSystem::init(std::size_t capacity)
{
  m_bulletManagerP.init(capacity);
  m_bulletManagerR.init(capacity);
}

template <typename BulletMotion>
void BulletSystem::spawnBullet(BulletPosition pos, BulletMotion motion, BulletMetadata meta)
{
  if constexpr (std::is_same_v<BulletMotion, BulletMotionP>) {
    m_bulletManagerP.spawnBullet(pos, motion, meta);
  } else if constexpr (std::is_same_v<BulletMotion, BulletMotionR>) {
    m_bulletManagerR.spawnBullet(pos, motion, meta);
  } else {
    static_assert(false, "Unsupported BulletMotion type");
  }
}

inline void BulletSystem::update(float screenWidth, float screenHeight)
{
  m_bulletManagerP.update(screenWidth, screenHeight);
  m_bulletManagerR.update(screenWidth, screenHeight);
}

inline void BulletSystem::clearBullets()
{
  m_bulletManagerP.clearBullets();
  m_bulletManagerR.clearBullets();
}
} // namespace Game
