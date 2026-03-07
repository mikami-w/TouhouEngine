#pragma once

#include "BulletManager.hpp"
#include "Core/BasicTypes.hpp"

namespace Game {
class BulletEmitter
{
public:
private:
  BulletManager* m_bulletManager = nullptr;
  Core::Position m_pos; // 发射源位置
};
} // namespace Game
