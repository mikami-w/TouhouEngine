#include "BulletManager.hpp"

#include <string>

#include "Bullet.hpp"
#include "Core/Logger.hpp"
#include "Graphics/SpriteRenderer.hpp"
#include "Graphics/TextureManager.hpp"

namespace Game {

BulletManager::BulletManager(std::size_t capacity)
  : m_handleSystem(capacity)
  , m_bulPos(capacity)
  , m_bulMotionP(capacity)
  , m_bulMotionR(capacity)
  , m_bulMeta(capacity, BulletMetadata{})
{
  LOG_INFO("BulletManager initialized with capacity: " + std::to_string(capacity));
}

BulletManager::Handle BulletManager::spawnBullet(Core::Vector2 pos,
                                                 MotionModule const& bulletMotion,
                                                 BulletMetadata meta) noexcept
{
  if (m_bulPos.x.size() <= m_activeCount) {
    std::uint32_t oldSize = m_bulPos.x.size();
    LOG_WARN(
      "BulletManager capacity reached (" + std::to_string(oldSize) +
      "), triggering auto-expansion to 2x. Consider increase default max capacity to avoid runtime reallocations.");
    resize(oldSize * 2); // 自动扩容
  }

  auto handle = m_handleSystem.createHandle(m_activeCount);
  m_bulPos.set(m_activeCount, pos);
  m_bulMotionP.set(m_activeCount, bulletMotion.polar);
  m_bulMotionR.set(m_activeCount, bulletMotion.rect);
  m_bulMeta[m_activeCount] = meta;
  ++m_activeCount;

  return handle;
}

void BulletManager::cancelBullet(Handle handle, bool spawnBonus) noexcept
{
  std::uint32_t bulletIdx = getBulletIndex(handle);
  if (!isBulletValid(bulletIdx)) {
    return;
  }
  // TODO: lifetime清零等消弹逻辑
  m_bulPos.set(bulletIdx, { -1000.0f, -1000.0f }); // 把子弹移出屏幕
  // 这里不直接作废句柄, 而是等待 update 中统一回收并执行 swap and pop
}

void BulletManager::setMotionModule(Handle handle, MotionModule newMotion) noexcept
{
  std::uint32_t bulletIdx = getBulletIndex(handle);
  if (!isBulletValid(bulletIdx)) {
    return;
  }
  m_bulMotionP.set(bulletIdx, newMotion.polar);
  m_bulMotionR.set(bulletIdx, newMotion.rect);
}

void BulletManager::setPosition(Handle handle, Core::Vector2 pos) noexcept
{
  std::uint32_t bulletIdx = getBulletIndex(handle);
  if (!isBulletValid(bulletIdx)) {
    return;
  }
  m_bulPos.set(bulletIdx, pos);
}

void BulletManager::setMetadata(Handle handle, BulletMetadata meta) noexcept
{
  std::uint32_t bulletIdx = getBulletIndex(handle);
  if (!isBulletValid(bulletIdx)) {
    return;
  }
  m_bulMeta[bulletIdx] = meta;
}

void BulletManager::update(float screenWidth, float screenHeight) noexcept
{
  static constexpr float offscreenMargin = 100.0f; // 允许子弹稍微出界一些再回收
  float const leftBound = -offscreenMargin;
  float const rightBound = screenWidth + offscreenMargin;
  float const topBound = -offscreenMargin;
  float const bottomBound = screenHeight + offscreenMargin;

  // 更新位置
  updatePosition();
  // 边界检查
  for (std::uint32_t i = 0; i < m_activeCount;) {
    if (m_bulPos.x[i] < leftBound || m_bulPos.x[i] > rightBound || m_bulPos.y[i] < topBound ||
        m_bulPos.y[i] > bottomBound) {
      // Swap and Pop 回收子弹: 用最后一颗子弹覆盖当前子弹, 然后减少有效子弹计数
      --m_activeCount;
      m_bulPos.moveTo(m_activeCount, i);
      m_bulMotionP.moveTo(m_activeCount, i);
      m_bulMotionR.moveTo(m_activeCount, i);
      m_bulMeta[i] = m_bulMeta[m_activeCount];
      m_handleSystem.notifySwapAndPop(i, m_activeCount); // 通知句柄系统当前被回收的子弹索引和被交换过来的子弹索引
      // 无需 ++i, 因为需要立刻检查被交换过来的子弹是否也需要回收
    } else {
      ++i; // 只有当子弹没有被回收时才移动到下一个索引
    }
  }
}

void BulletManager::draw(Graphics::SpriteRenderer* sr, Graphics::TextureManager* tm) const
{
  namespace Math = Core::Math;

  // 暂时复用八云紫的贴图来当做子弹, 缩小到 20x20
  auto tama_large = tm->getTexture("tama_large");

  for (std::uint32_t i = 0; i < m_activeCount; ++i) {
    float angle = 0;

    sr->drawSprite(tama_large,
                   {m_bulPos.x[i], m_bulPos.y[i]},
                   angle,           // 子弹总是面向运动方向
                   { 20.0f, 20.0f } // 子弹大小
    );
  }
}

void BulletManager::clearBullets() noexcept
{
  m_activeCount = 0;
  m_handleSystem.reset();
}

void BulletManager::updatePosition() noexcept
{
  namespace Math = Core::Math;

  float* __restrict px = m_bulPos.x.data();
  float* __restrict py = m_bulPos.y.data();

  float* __restrict pAng = m_bulMotionP.angle.data();
  float* __restrict pAngVel = m_bulMotionP.angVel.data();
  float* __restrict pAngAccel = m_bulMotionP.angAccel.data();
  float* __restrict pTanSpd = m_bulMotionP.speed.data();
  float* __restrict pTanAccel = m_bulMotionP.tanAccel.data();

  float* __restrict pVelX = m_bulMotionR.vel_x.data();
  float* __restrict pVelY = m_bulMotionR.vel_y.data();
  float* __restrict pAccelX = m_bulMotionR.accel_x.data();
  float* __restrict pAccelY = m_bulMotionR.accel_y.data();

#pragma omp simd
  for (std::uint32_t i = 0; i < m_activeCount; ++i) {
    // 中点角度法 (Midpoint Angle Integration)
    // 记录旧角度 (为了计算中点)
    float oldAng = pAng[i];

    // 更新速度与角度 (维持半隐式欧拉)
    pAngVel[i] += pAngAccel[i];
    pAng[i] += pAngVel[i]; // 此时 pAng[i] 已经是 theta_1 了
    pTanSpd[i] += pTanAccel[i];
    pVelX[i] += pAccelX[i];
    pVelY[i] += pAccelY[i];

    // 计算中点角度
    // 中点角度 theta_mid = (oldAng + pAng[i]) * 0.5f
    // 也可以写成：theta_mid = oldAng + pAngVel[i] * 0.5f;
    float midAng = oldAng + pAngVel[i] * 0.5f;

    // 使用中点角度更新位置
    px[i] += pTanSpd[i] * Math::cos(midAng) + pVelX[i];
    py[i] += pTanSpd[i] * Math::sin(midAng) + pVelY[i];
  }
}

void BulletManager::resize(std::uint32_t newCapacity) noexcept
{
  m_handleSystem.resize(newCapacity);
  m_bulPos.resize(newCapacity);
  m_bulMotionP.resize(newCapacity);
  m_bulMotionR.resize(newCapacity);
  m_bulMeta.resize(newCapacity);
}
} // namespace Game
