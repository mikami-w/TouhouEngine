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
  BulletSystem(std::size_t capacity);
  ~BulletSystem() = default;

  template <typename BulletMotion>
  void spawnBullet(BulletPosition pos, BulletMotion motion, BulletMetadata meta);

  void update(float screenWidth, float screenHeight);
  void draw(Graphics::SpriteRenderer* sr, Graphics::TextureManager* tm) const;

  void clearBullets();

private:
  BulletManager<BulletMotionP> m_bulletManagerP;
  BulletManager<BulletMotionR> m_bulletManagerR;

  // 后续加入激光等子弹类型时, 可以继续添加对应的 BulletManager 实例
};

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

inline void BulletSystem::draw(Graphics::SpriteRenderer* sr, Graphics::TextureManager* tm) const
{
  m_bulletManagerP.draw(sr, tm);
  m_bulletManagerR.draw(sr, tm);
}

inline void BulletSystem::clearBullets()
{
  m_bulletManagerP.clearBullets();
  m_bulletManagerR.clearBullets();
}
} // namespace Game
