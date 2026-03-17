#pragma once

#include <memory>

#include "Core/BasicTypes.hpp"

namespace Graphics {
class Texture;
} // namespace Graphics

namespace Game {
class Entity
{
public:
  Entity() = default;
  Entity(Core::Position position);
  ~Entity() = default;

  Core::Position getPosition() const noexcept { return m_position; }
  void setPosition(Core::Position position) noexcept { m_position = position; }

  void update();

private:
  Core::Position m_position{};            // 实体在游戏世界中的位置
  Graphics::Texture* m_texture = nullptr; // 仅观察, 不拥有资源; nullptr 表示无纹理, 仅作为发弹点等依附的移动轨迹

  // 待添加: 移动
};
} // namespace Game

#include "Entity.ipp"
