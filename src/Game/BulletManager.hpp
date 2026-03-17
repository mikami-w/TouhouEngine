#pragma once

#include <cstdint>
#include <vector>

#include "Game/Bullet.hpp"

namespace Graphics {
class SpriteRenderer;
class TextureManager;
} // namespace Graphics

namespace Game {
template <typename BulletMotion>
class BulletManager
{
public:
  struct BulletBuffer
  {
    BulletPosition const* bulPos = nullptr;
    BulletMotion const* bulMotion = nullptr;
    BulletMetadata const* bulMeta = nullptr;
  };

public:
  BulletManager(std::size_t capacity);
  ~BulletManager() = default;

  BulletManager(BulletManager const&) = delete;
  BulletManager& operator=(BulletManager const&) = delete;
  BulletManager(BulletManager&&) = delete;
  BulletManager& operator=(BulletManager&&) = delete;

  // 创造一颗新子弹
  void spawnBullet(BulletPosition pos, BulletMotion const& bulletMotion, BulletMetadata meta) noexcept;

  // 每帧调用, 更新位置并回收出界子弹
  void update(float screenWidth, float screenHeight) noexcept;

  // 遍历并绘制
  void draw(Graphics::SpriteRenderer* sr, Graphics::TextureManager* tm) const;

  // 清空全屏子弹
  void clearBullets() noexcept { m_activeCount = 0; }

  // 获取有效子弹数据 buffer
  BulletBuffer getActiveBullets() const noexcept;
  int getActiveCount() const noexcept { return m_activeCount; }

private:
  std::vector<BulletPosition> m_bulPos;
  std::vector<BulletMotion> m_bulMotion;
  std::vector<BulletMetadata> m_bulMeta;
  int m_activeCount = 0;
};
} // namespace Game

#include "BulletManager.tpp"
