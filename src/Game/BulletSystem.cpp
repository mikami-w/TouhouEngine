#include "BulletSystem.hpp"

#include "Graphics/SpriteRenderer.hpp"
#include "Graphics/TextureManager.hpp"

namespace Game {

void BulletSystem::draw(Graphics::SpriteRenderer* sr, Graphics::TextureManager* tm)
{
  namespace Math = Core::Math;

  auto bullets = m_bulletManagerP.getActiveBullets();
  int count = m_bulletManagerP.getActiveCount();

  // 暂时复用八云紫的贴图来当做子弹, 缩小到 20x20
  auto yukari = tm->getTexture("yukari");
  for (int i = 0; i < count; ++i) {
    sr->drawSprite(yukari,
                   bullets.bulPos[i],
                   bullets.bulMotion[i].angle - Math::PI_2_f, // 子弹总是面向运动方向
                   { 20.0f, 20.0f }                           // 子弹大小
    );
  }
}
} // namespace Game
