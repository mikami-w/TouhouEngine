#pragma once

#include <cstdint>

#include "BulletRouter.hpp"
#include "Core/EngineSettings.hpp"
#include "EntityManager.hpp"

namespace Graphics {
class TextureManager;
class SpriteRenderer;
}

namespace Game {
class World
{
public:
  explicit World(Graphics::TextureManager* textureManager);
  ~World() = default;

  World(World const&) = delete;
  World& operator=(World const&) = delete;

  void reset();                                                  // 游戏世界的初始化 (重置关卡)
  void update(Core::Config::PlayAreaSettings& playAreaSettings); // 游戏主循环推演
  void draw(Graphics::SpriteRenderer* renderer) const;           // 提交渲染指令, 把画笔递给世界, 让世界自己画自己

private:
  Graphics::TextureManager* m_textureManager; // 仅观察, 不拥有纹理资源
  BulletRouter m_bulletRouter;
  EntityManager m_entityManager;

  std::uint64_t m_logicalFrame = 0; // 游戏内的绝对时间轴

  void _temp_logic_to_be_replaced(Core::Config::PlayAreaSettings& playAreaSettings);
};
}
