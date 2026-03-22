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
namespace detail {
template <typename MotionT>
struct BulletMotionTypeSelector
{};

template <>
struct BulletMotionTypeSelector<BulletMotionP>
{
  inline static constexpr BulletMotionType type = BulletMotionType::Polar;
};

template <>
struct BulletMotionTypeSelector<BulletMotionR>
{
  inline static constexpr BulletMotionType type = BulletMotionType::Rectangular;
};

template <typename MotionT>
inline constexpr BulletMotionType BULLET_MOTION_TYPE = BulletMotionTypeSelector<MotionT>::type;
} // namespace detail

struct BulletHandle
{
  Core::SAPHandleSystem::Handle handle{};
  BulletMotionType type{};
};

template <typename BulletMotion>
class BulletManager
{
public:
  struct BulletBuffer
  {
    Position const* bulPos = nullptr;
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
  BulletHandle spawnBullet(Position pos, BulletMotion const& bulletMotion, BulletMetadata meta) noexcept;
  BulletView getBullet(BulletHandle handle) noexcept;
  void cancelBullet(BulletHandle handle, bool spawnBonus = false) noexcept;

  // 每帧调用, 更新位置并回收出界子弹
  void update(float screenWidth, float screenHeight) noexcept;

  // 遍历并绘制
  void draw(Graphics::SpriteRenderer* sr, Graphics::TextureManager* tm) const;

  // 清空全屏子弹
  void clearBullets() noexcept;

  // 获取有效子弹数据 buffer
  BulletBuffer getActiveBullets() const noexcept;
  std::uint32_t getActiveCount() const noexcept { return m_activeCount; }

private:
  Core::SAPHandleSystem m_handleSystem;
  std::vector<Position> m_bulPos;
  std::vector<BulletMotion> m_bulMotion;
  std::vector<BulletMetadata> m_bulMeta;
  std::uint32_t m_activeCount = 0;
};
} // namespace Game

#include "BulletManager.tpp"
