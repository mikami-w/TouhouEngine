#pragma once

#include "Game/Bullet.hpp"

#include <cstdint>
#include <vector>

namespace Game {
class BulletManager
{
public:
  BulletManager() = default;
  ~BulletManager() = default;

  BulletManager(BulletManager const&) = delete;
  BulletManager& operator=(BulletManager const&) = delete;
  BulletManager(BulletManager&&) = delete;

  void init(std::size_t capacity); // 初始化内存池大小

  // 创造一颗新子弹
  void spawnBullet(Bullet const& bullet) noexcept;
  // 使用矢量速度创造子弹
  void spawnBulletV(float x, float y, float vx, float vy, std::uint16_t type, std::uint16_t color) noexcept;
  // 使用角(弧度制)和速率创造子弹
  void spawnBulletA(float x, float y, float angle, float v, std::uint16_t type, std::uint16_t color) noexcept;

  // 每帧调用, 更新位置并回收出界子弹
  void update(float screenWidth, float screenHeight);

  // 清空全屏子弹
  void clearBullets();

  // 获取有效子弹数据 buffer
  Bullet const* getActiveBullets() const noexcept { return m_bullets.data(); }
  std::size_t getActiveCount() const noexcept { return m_activeCount; }

private:
  std::vector<Bullet> m_bullets;
  std::size_t m_activeCount;
};
} // namespace Game
