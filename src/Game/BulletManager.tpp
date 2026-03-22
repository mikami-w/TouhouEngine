#include "Bullet.hpp"

#include <string>

#include "Core/Logger.hpp"
#include "Graphics/SpriteRenderer.hpp"
#include "Graphics/TextureManager.hpp"

namespace Game {

template <SimpleMotion BulletMotion>
BulletManager<BulletMotion>::BulletManager(std::size_t capacity)
  : m_handleSystem(capacity)
{
  m_bulPos.resize(capacity);
  m_bulMotion.resize(capacity);
  m_bulMeta.resize(capacity);
  LOG_INFO("BulletManager initialized with capacity: " + std::to_string(capacity));
}

template <SimpleMotion BulletMotion>
BulletHandle BulletManager<BulletMotion>::spawnBullet(Position pos,
                                                      BulletMotion const& bulletMotion,
                                                      BulletMetadata meta) noexcept
{
  if (m_bulPos.size() <= m_activeCount) {
    std::uint32_t oldSize = m_bulPos.size();
    LOG_WARN(
      "BulletManager capacity reached (" + std::to_string(oldSize) +
      "), triggering auto-expansion to 2x. Consider increase default max capacity to avoid runtime reallocations.");
    m_bulPos.resize(oldSize * 2); // 自动扩容
    m_bulMotion.resize(oldSize * 2);
    m_bulMeta.resize(oldSize * 2);
    m_handleSystem.resize(oldSize * 2);
  }

  auto handle = m_handleSystem.createHandle(m_activeCount);
  m_bulPos[m_activeCount] = pos;
  m_bulMotion[m_activeCount] = bulletMotion;
  m_bulMeta[m_activeCount] = meta;
  ++m_activeCount;

  return { handle, detail::BULLET_MOTION_TYPE<BulletMotion> };
}

template <SimpleMotion BulletMotion>
BulletView BulletManager<BulletMotion>::getBullet(BulletHandle handle) noexcept
{
#if defined(_DEBUG)
  if (handle.type != detail::BULLET_MOTION_TYPE<BulletMotion>) {
    LOG_ERROR("BulletHandle motion type mismatch when accessing BulletManager. Expected: " +
              std::to_string(static_cast<int>(detail::BULLET_MOTION_TYPE<BulletMotion>)) +
              ", Actual: " + std::to_string(static_cast<int>(handle.type)));
    return {};
  }
#endif
  std::uint32_t index = m_handleSystem.toDataIndex(handle.handle);
  return index != Core::SAPHandleSystem::INVALID_INDEX ? BulletView{ detail::BULLET_MOTION_TYPE<BulletMotion>,
                                                                     &m_bulPos[index],
                                                                     &m_bulMeta[index],
                                                                     { .raw = &m_bulMotion[index] } }
                                                       : BulletView{};
}

template <SimpleMotion BulletMotion>
void BulletManager<BulletMotion>::cancelBullet(BulletHandle handle, bool spawnBonus) noexcept
{
  BulletView bullet = getBullet(handle);
  if (!bullet) {
    return;
  }
  // TODO: lifetime清零等消弹逻辑
  *bullet.position = { -1000.0f, -1000.0f }; // 把子弹移出屏幕
  // 这里不直接作废句柄, 而是等待 update 中统一回收并执行 swap and pop
}

template <SimpleMotion BulletMotion>
void BulletManager<BulletMotion>::update(float screenWidth, float screenHeight) noexcept
{
  static constexpr float offscreenMargin = 100.0f; // 允许子弹稍微出界一些再回收
  float const leftBound = -offscreenMargin;
  float const rightBound = screenWidth + offscreenMargin;
  float const topBound = -offscreenMargin;
  float const bottomBound = screenHeight + offscreenMargin;

  for (std::uint32_t i = 0; i < m_activeCount;) {
    Position& pos = m_bulPos[i];
    BulletMotion& motion = m_bulMotion[i];

    // 更新位置
    motion.updatePosition(pos);
    // 边界检查
    if (pos.x < leftBound || pos.x > rightBound || pos.y < topBound || pos.y > bottomBound) {
      // Swap and Pop 回收子弹: 用最后一颗子弹覆盖当前子弹, 然后减少有效子弹计数
      --m_activeCount;
      m_bulPos[i] = m_bulPos[m_activeCount];
      m_bulMotion[i] = m_bulMotion[m_activeCount];
      m_bulMeta[i] = m_bulMeta[m_activeCount];
      m_handleSystem.notifySwapAndPop(i, m_activeCount); // 通知句柄系统当前被回收的子弹索引和被交换过来的子弹索引
      // 无需 ++i, 因为需要立刻检查被交换过来的子弹是否也需要回收
    } else {
      ++i; // 只有当子弹没有被回收时才移动到下一个索引
    }
  }
}

template <SimpleMotion BulletMotion>
void BulletManager<BulletMotion>::draw(Graphics::SpriteRenderer* sr, Graphics::TextureManager* tm) const
{
  namespace Math = Core::Math;

  auto bullets = getActiveBullets();

  // 暂时复用八云紫的贴图来当做子弹, 缩小到 20x20
  auto yukari = tm->getTexture("yukari");

  for (std::uint32_t i = 0; i < m_activeCount; ++i) {
    float angle;
    if constexpr (std::is_same_v<BulletMotion, BulletMotionP>) {
      // 子弹贴图旋转角度 = 运动方向角度 - 90 度 (因为默认贴图朝上, 需要调整)
      angle = bullets.bulMotion[i].angle - Math::PI_2_f;
    } else if constexpr (std::is_same_v<BulletMotion, BulletMotionR>) {
      // 计算运动方向角度
      angle = Math::atan2(bullets.bulMotion[i].vel.y, bullets.bulMotion[i].vel.x);
    } else {
      static_assert(false, "Unsupported BulletMotion type");
    }
    sr->drawSprite(yukari,
                   bullets.bulPos[i],
                   angle,           // 子弹总是面向运动方向
                   { 20.0f, 20.0f } // 子弹大小
    );
  }
}

template <SimpleMotion BulletMotion>
void BulletManager<BulletMotion>::clearBullets() noexcept
{
  m_activeCount = 0;
  m_handleSystem.reset();
}

template <SimpleMotion BulletMotion>
typename BulletManager<BulletMotion>::BulletBuffer BulletManager<BulletMotion>::getActiveBullets() const noexcept
{
  return { m_bulPos.data(), m_bulMotion.data(), m_bulMeta.data() };
}
} // namespace Game
