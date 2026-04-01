#pragma once

#include <cstdint>
#include <vector>

#include "Core/SAPHandleSystem.hpp"
#include "Game/Bullet.hpp"

namespace Graphics {
class SpriteRenderer;
class TextureManager;
} // namespace Graphics

namespace Game {

class BulletManager
{
public:
  using Handle = Core::SAPHandleSystem::Handle;

public:
  BulletManager(std::size_t capacity);
  ~BulletManager() = default;

  BulletManager(BulletManager const&) = delete;
  BulletManager& operator=(BulletManager const&) = delete;
  BulletManager(BulletManager&&) = delete;
  BulletManager& operator=(BulletManager&&) = delete;

  // 创造一颗新子弹
  Handle spawnBullet(Core::Vector2 pos, MotionModule const& bulletMotion, BulletMetadata meta) noexcept;
  std::uint32_t getBulletIndex(Handle handle) noexcept { return m_handleSystem.toDataIndex(handle); }
  void cancelBullet(Handle handle, bool spawnBonus = false) noexcept;

  void setMotionModule(Handle handle, MotionModule newMotion) noexcept;
  void setPosition(Handle handle, Core::Vector2 pos) noexcept;
  void setMetadata(Handle handle, BulletMetadata meta) noexcept;


  // 每帧调用
  void update(float screenWidth, float screenHeight) noexcept; // 更新位置并回收出界子弹
  void draw(Graphics::SpriteRenderer* sr, Graphics::TextureManager* tm) const; // 遍历并绘制

  // 清空全屏子弹
  void clearBullets() noexcept;

  std::uint32_t getActiveCount() const noexcept { return m_activeCount; }

private:
  void updatePosition() noexcept;
  void resize(std::uint32_t newCapacity) noexcept;
  bool isBulletValid(std::uint32_t bulletIdx) noexcept { return bulletIdx < m_activeCount; }

private:
  Core::SAPHandleSystem m_handleSystem;
  PositionArray m_bulPos;
  MotionArray_Polar m_bulMotionP;
  MotionArray_Rectangular m_bulMotionR;
  std::vector<BulletMetadata> m_bulMeta;
  std::uint32_t m_activeCount = 0;
};
} // namespace Game
