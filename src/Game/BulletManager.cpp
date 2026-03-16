#include "BulletManager.hpp"

#include <string>

#include "Core/Logger.hpp"

namespace Game {

void BulletManager::init(std::size_t capacity)
{
  m_bulPos.resize(capacity);
  m_bulMotion.resize(capacity);
  m_bulMeta.resize(capacity);

  m_activeCount = 0;
  LOG_INFO("BulletManager initialized with capacity: " + std::to_string(capacity));
}

void BulletManager::spawnBullet(BulletPosition pos, BulletMotionP const& bulletMotion, BulletMetadata meta) noexcept
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

void BulletManager::update(float screenWidth, float screenHeight)
{
  static constexpr float offscreenMargin = 100.0f; // 允许子弹稍微出界一些再回收
  float const leftBound = -offscreenMargin;
  float const rightBound = screenWidth + offscreenMargin;
  float const topBound = -offscreenMargin;
  float const bottomBound = screenHeight + offscreenMargin;

  for (int i = 0; i < m_activeCount;) {
    BulletPosition& pos = m_bulPos[i];
    BulletMotionP& motion = m_bulMotion[i];
    BulletMetadata& meta = m_bulMeta[i];

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
BulletManager::BulletBuffer BulletManager::getActiveBullets() const noexcept
{
  return { m_bulPos.data(), m_bulMotion.data(), m_bulMeta.data() };
}
} // namespace Game
