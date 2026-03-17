#include <string>

#include "Core/Logger.hpp"
#include "Graphics/SpriteRenderer.hpp"
#include "Graphics/TextureManager.hpp"

namespace Game {

template <typename BulletMotion>
BulletManager<BulletMotion>::BulletManager(std::size_t capacity)
  : m_activeCount(0)
{
  m_bulPos.resize(capacity);
  m_bulMotion.resize(capacity);
  m_bulMeta.resize(capacity);
  LOG_INFO("BulletManager initialized with capacity: " + std::to_string(capacity));
}

template <typename BulletMotion>
void BulletManager<BulletMotion>::spawnBullet(BulletPosition pos,
                                              BulletMotion const& bulletMotion,
                                              BulletMetadata meta) noexcept
{
  if (m_bulPos.size() <= m_activeCount) {
    LOG_WARN("BulletManager capacity reached. Cannot spawn more bullets.");
    return;
  }

  m_bulPos[m_activeCount] = pos;
  m_bulMotion[m_activeCount] = bulletMotion;
  m_bulMeta[m_activeCount] = meta;

  ++m_activeCount;
}

template <typename BulletMotion>
void BulletManager<BulletMotion>::update(float screenWidth, float screenHeight) noexcept
{
  static constexpr float offscreenMargin = 100.0f; // 允许子弹稍微出界一些再回收
  float const leftBound = -offscreenMargin;
  float const rightBound = screenWidth + offscreenMargin;
  float const topBound = -offscreenMargin;
  float const bottomBound = screenHeight + offscreenMargin;

  for (int i = 0; i < m_activeCount;) {
    BulletPosition& pos = m_bulPos[i];
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
      // 无需 ++i, 因为需要立刻检查被交换过来的子弹是否也需要回收
    } else {
      ++i; // 只有当子弹没有被回收时才移动到下一个索引
    }
  }
}

template <typename BulletMotion>
void BulletManager<BulletMotion>::draw(Graphics::SpriteRenderer* sr, Graphics::TextureManager* tm) const
{
  namespace Math = Core::Math;

  auto bullets = getActiveBullets();

  // 暂时复用八云紫的贴图来当做子弹, 缩小到 20x20
  auto yukari = tm->getTexture("yukari");

  for (int i = 0; i < m_activeCount; ++i) {
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

template <typename BulletMotion>
typename BulletManager<BulletMotion>::BulletBuffer BulletManager<BulletMotion>::getActiveBullets() const noexcept
{
  return { m_bulPos.data(), m_bulMotion.data(), m_bulMeta.data() };
}
} // namespace Game
