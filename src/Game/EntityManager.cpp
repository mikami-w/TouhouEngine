#include "EntityManager.hpp"

#include <vector>

#include "Core/Logger.hpp"
#include "Graphics/Texture.hpp"

namespace Game {

EntityManager::EntityManager(std::size_t capacity)
  : m_handleSystem(capacity)
{
  m_entities.resize(capacity);
  m_activeCount = 0;
  LOG_INFO("EntityManager initialized with capacity: " + std::to_string(capacity));
}

EntityManager::Handle EntityManager::spawnEntity(Entity entity) noexcept
{
  if (m_entities.size() <= m_activeCount) {
    std::uint32_t oldSize = m_entities.size();
    LOG_WARN(
      "EntityManager capacity reached (" + std::to_string(oldSize) +
      "), triggering auto-expansion to 2x. Consider increase default max capacity to avoid runtime reallocations.");
    m_entities.resize(oldSize * 2); // 自动扩容
    m_handleSystem.resize(oldSize * 2);
  }

  Handle handle = m_handleSystem.createHandle(m_activeCount);
  m_entities[m_activeCount++] = entity;

  return handle;
}

Entity* EntityManager::getEntity(Handle handle) noexcept
{
  std::uint32_t index = m_handleSystem.toDataIndex(handle);
  return index != Core::SAPHandleSystem::INVALID_INDEX ? &m_entities[index] : nullptr;
}

void EntityManager::update(float screenWidth, float screenHeight)
{
  static constexpr float offscreenMargin = 100.0f; // 允许子弹稍微出界一些再回收
  float const leftBound = -offscreenMargin;
  float const rightBound = screenWidth + offscreenMargin;
  float const topBound = -offscreenMargin;
  float const bottomBound = screenHeight + offscreenMargin;

  for (std::uint32_t i = 0; i < m_activeCount;) {
    Entity& ei = m_entities[i];

    // 更新位置
    ei.update();
    // 边界检查
    if (ei.getPosition().x < leftBound || ei.getPosition().x > rightBound || ei.getPosition().y < topBound ||
        ei.getPosition().y > bottomBound) {
      // Swap and Pop
      m_entities[i] = m_entities[--m_activeCount];
      m_handleSystem.notifySwapAndPop(i, m_activeCount);
      // 无需 ++i, 因为需要立刻检查被交换过来的子弹是否也需要回收
    } else {
      ++i; // 只有当子弹没有被回收时才移动到下一个索引
    }
  }
}

void EntityManager::draw(Graphics::SpriteRenderer* sr, Graphics::TextureManager* tm) const
{
  namespace Math = Core::Math;

  auto entities = getActiveEntities();

  // 暂时复用八云紫的贴图
  auto yukari = tm->getTexture("yukari");

  for (std::uint32_t i = 0; i < m_activeCount; ++i) {
    sr->drawSprite(yukari, entities[i].getPosition(), 0.0f, { yukari->getWidth() / 4.0f, yukari->getHeight() / 4.0f });
  }
}
} // namespace Game
