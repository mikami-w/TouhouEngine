#include "BulletSystem.hpp"

#include "Graphics/SpriteRenderer.hpp"
#include "Graphics/TextureManager.hpp"

namespace Game {

BulletSystem::BulletSystem(std::size_t capacity)
  : m_bulletManagerP(capacity)
  , m_bulletManagerR(capacity)
{
}
} // namespace Game
