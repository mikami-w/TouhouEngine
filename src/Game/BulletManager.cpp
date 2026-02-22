#include "BulletManager.hpp"
#include "Core/Logger.hpp"
#include "Core/MathUtils.hpp"

#include <string>

namespace Game {

void BulletManager::init(std::size_t capacity)
{
  m_bullets.resize(capacity);
  m_activeCount = 0;
  LOG_INFO("BulletManager initialized with capacity: " + std::to_string(capacity));
}

void BulletManager::spawnBullet(Bullet const& bullet) noexcept
{
  if (m_bullets.size() <= m_activeCount) {
    LOG_WARN("BulletManager capacity reached. Cannot spawn more bullets.");
    return;
  }

  m_bullets[m_activeCount++] = bullet;
}

void BulletManager::spawnBulletV(float x,
                                 float y,
                                 float vx,
                                 float vy,
                                 float accelx,
                                 float accely,
                                 std::uint16_t type,
                                 std::uint16_t color) noexcept
{
  float angle = std::atan2(vx, vy);           // 与 +y 的夹角
  float speed = std::sqrt(vx * vx + vy * vy); // 速率
  // TODO: 计算角加速度和切向加速度
  // spawnBullet({ });
  LOG_DEBUG("WARNING! BulletManager::spawnBulletV is called. This may cause extra overhead due to atan2 and sqrt "
            "calculations.");
}

void BulletManager::spawnBulletA(float x,
                                 float y,
                                 float angle,
                                 float angVel,
                                 float angAccel,
                                 float speed,
                                 float tanAccel,
                                 std::uint16_t type,
                                 std::uint16_t color) noexcept
{
  spawnBullet({ x, y, angle, angVel, angAccel, speed, tanAccel, type, color });
}

void BulletManager::update(float screenWidth, float screenHeight)
{
  static constexpr float offscreenMargin = 100.0f; // 允许子弹稍微出界一些再回收
  float const leftBound = -offscreenMargin;
  float const rightBound = screenWidth + offscreenMargin;
  float const topBound = -offscreenMargin;
  float const bottomBound = screenHeight + offscreenMargin;

  for (int i = 0; i < m_activeCount;) {
    Bullet& b = m_bullets[i];
    // 更新位置
    b.updatePosition();
    // 边界检查
    if (b.x < leftBound || b.x > rightBound || b.y < topBound || b.y > bottomBound) {
      // Swap and Pop 回收子弹: 用最后一颗子弹覆盖当前子弹, 然后减少有效子弹计数
      m_bullets[i] = m_bullets[--m_activeCount];
      // 无需 ++i, 因为需要立刻检查被交换过来的子弹是否也需要回收
    } else {
      ++i; // 只有当子弹没有被回收时才移动到下一个索引
    }
  }
}

void BulletManager::clearBullets()
{
  m_activeCount = 0;
}
} // namespace Game
