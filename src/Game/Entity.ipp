#include <memory>

#include "BulletEmitter.hpp"
#include "BulletManager.hpp"

namespace Game {

inline Entity::Entity(Core::Vector2 position)
  : m_position(position)
{
}

inline void Entity::update() {}
} // namespace Game
