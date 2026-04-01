#include "BulletRouter.hpp"

#include "Graphics/SpriteRenderer.hpp"
#include "Graphics/TextureManager.hpp"

namespace Game {

inline BulletRouter::BulletRouter(std::size_t capacity)
  : m_bulletManagerP(capacity)
  , m_bulletManagerR(capacity)
{
}

template <SimpleMotion BulletMotion>
BulletHandle BulletRouter::spawnBullet(Core::Vector2 pos, BulletMotion motion, BulletMetadata meta) noexcept
{
  if constexpr (std::is_same_v<BulletMotion, BulletMotionP>) {
    return m_bulletManagerP.spawnBullet(pos, motion, meta);
  } else if constexpr (std::is_same_v<BulletMotion, BulletMotionR>) {
    return m_bulletManagerR.spawnBullet(pos, motion, meta);
  } else {
    static_assert(false, "Unsupported BulletMotion type");
  }
  return {};
}

template <SimpleMotion BulletMotion>
void BulletRouter::setMotionModule(BulletHandle& handle, BulletMotion newMotion) noexcept
{
  BulletView view = getBullet(handle);
  if (!view) {
    return; // 无效视图
  }

  if (handle.type == BulletMotionType::Polar) {
    if constexpr (std::is_same_v<BulletMotion, BulletMotionP>) {
      *(view.motion.polar) = newMotion; // 类型相同直接修改动作模组
    } else {
      auto oldHandle = handle;
      handle = spawnBullet(*view.position, newMotion, *view.metadata); // 使用新的动作模组生成新子弹并修改句柄
      cancelBullet(oldHandle, false);                                  // 取消旧子弹
    }
  } else if (handle.type == BulletMotionType::Rectangular) {
    if constexpr (std::is_same_v<BulletMotion, BulletMotionR>) {
      *(view.motion.rectangular) = newMotion; // 类型相同直接修改动作模组
    } else {
      auto oldHandle = handle;
      handle = spawnBullet(*view.position, newMotion, *view.metadata); // 使用新的动作模组生成新子弹并修改句柄
      cancelBullet(oldHandle, false);                                  // 取消旧子弹
    }
  }
}

inline BulletView BulletRouter::getBullet(BulletHandle handle)
{
  if (handle.type == BulletMotionType::Polar) {
    return m_bulletManagerP.getBullet(handle);
  } else if (handle.type == BulletMotionType::Rectangular) {
    return m_bulletManagerR.getBullet(handle);
  } else {
    LOG_ERROR("BulletHandle motion type mismatch when routing to BulletManager. Handle type: " +
              std::to_string(static_cast<int>(handle.type)));
    return {};
  }
}

inline void BulletRouter::cancelBullet(BulletHandle handle, bool spawnBonus) noexcept
{
  if (handle.type == BulletMotionType::Polar) {
    m_bulletManagerP.cancelBullet(handle, spawnBonus);
  } else if (handle.type == BulletMotionType::Rectangular) {
    m_bulletManagerR.cancelBullet(handle, spawnBonus);
  } else {
    LOG_ERROR("BulletHandle motion type mismatch when routing to BulletManager. Handle type: " +
              std::to_string(static_cast<int>(handle.type)));
  }
}

inline void BulletRouter::update(float screenWidth, float screenHeight)
{
  m_bulletManagerP.update(screenWidth, screenHeight);
  m_bulletManagerR.update(screenWidth, screenHeight);
}

inline void BulletRouter::draw(Graphics::SpriteRenderer* sr, Graphics::TextureManager* tm) const
{
  m_bulletManagerP.draw(sr, tm);
  m_bulletManagerR.draw(sr, tm);
}

inline void BulletRouter::clearBullets()
{
  m_bulletManagerP.clearBullets();
  m_bulletManagerR.clearBullets();
}
} // namespace Game
