#pragma once

#include "Bullet.hpp"
#include "BulletManager.hpp"

namespace Graphics {
class SpriteRenderer;
class TextureManager;
}

namespace Game {
class BulletRouter
{
public:
  BulletRouter(std::size_t capacity);
  ~BulletRouter() = default;

  template <typename BulletMotion>
  void spawnBullet(Position pos, BulletMotion motion, BulletMetadata meta) noexcept;
  BulletView getBullet(BulletHandle handle);
  void cancelBullet(BulletHandle handle, bool spawnBonus = false) noexcept;

  void update(float screenWidth, float screenHeight);
  void draw(Graphics::SpriteRenderer* sr, Graphics::TextureManager* tm) const;

  void clearBullets();

private:
  BulletManager<BulletMotionP> m_bulletManagerP;
  BulletManager<BulletMotionR> m_bulletManagerR;

  // 后续加入激光等子弹类型时, 可以继续添加对应的 BulletManager 实例
};
} // namespace Game

#include "BulletRouter.ipp"
