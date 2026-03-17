#pragma once

#include <vector>

#include "Entity.hpp"

namespace Game {
class EntityManager
{
public:
  EntityManager(std::size_t capacity);
  ~EntityManager() = default;

  EntityManager(EntityManager const&) = delete;
  EntityManager& operator=(EntityManager const&) = delete;
  EntityManager(EntityManager&&) = delete;
  EntityManager& operator=(EntityManager&&) = delete;

  // 创造实体
  void spawnEntity(Entity entity) noexcept;

  // 每帧调用, 更新位置并回收出界实体
  void update(float screenWidth, float screenHeight);

  // 遍历并绘制
  void draw(Graphics::SpriteRenderer* sr, Graphics::TextureManager* tm) const;

  // 清空全屏实体
  void clearEntities() { m_activeCount = 0; }

  // 获取有效实体数据 buffer
  Entity const* getActiveEntities() const noexcept { return m_entities.data(); }
  int getActiveCount() const noexcept { return m_activeCount; }

private:
  std::vector<Entity> m_entities;
  int m_activeCount = 0;
};
} // namespace Game
