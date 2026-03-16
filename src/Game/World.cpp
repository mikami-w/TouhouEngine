#include "World.hpp"

#include "BulletEmitter.hpp"
#include "Core/Logger.hpp"
#include "Core/MathUtils.hpp"
#include "Graphics/SpriteRenderer.hpp"
#include "Graphics/TextureManager.hpp"

namespace Game {

World::World(Graphics::TextureManager* textureManager)
  : m_textureManager(textureManager)
{
  m_bulletSystem.init(50000 - 5000);    // 初始化弹幕池, 最多支持的子弹数取决于 SpriteRenderer 的实例缓冲区大小 (50000)
  BulletEmitter::init(&m_bulletSystem); // 依赖注入, 将 BulletManager 的地址传递给 BulletEmitter
  m_entityManager.init(20000);          // 初始化实体池
  reset();
  LOG_INFO("World initialized.");
}

void World::reset()
{
  m_logicalFrame = 0;
  m_bulletSystem.clearBullets();
  LOG_INFO("World reset: logical frame reset and bullets cleared.");
}

void World::update(Core::Config::PlayAreaSettings& playAreaSettings)
{
  namespace Math = Core::Math;
  ++m_logicalFrame;

  // 只执行一次, 在游戏区域边界生成一圈静止子弹
  static int const blackMagicHack_spawnEdge = [&] {
    BulletPosition pos{};
    while (pos.y <= playAreaSettings.logicalHeight) {
      m_bulletSystem.spawnBullet(pos, BulletMotionP{}, {});
      pos.x = playAreaSettings.logicalWidth;
      m_bulletSystem.spawnBullet(pos, BulletMotionP{}, {});
      pos.y += 10.0f;
      pos.x = 0.0f;
    }
    pos = {};
    while (pos.x <= playAreaSettings.logicalWidth) {
      m_bulletSystem.spawnBullet(pos, BulletMotionP{}, {});
      pos.y = playAreaSettings.logicalHeight;
      m_bulletSystem.spawnBullet(pos, BulletMotionP{}, {});
      pos.x += 10.0f;
      pos.y = 0.0f;
    }
    return 0;
  }();

  {
    // 生成子弹
    static constexpr double spawnAngAccel = 0.001f;
    static double spawnAngle = 0.0f;
    static double spawnAngVel = 0.0f;
    spawnAngVel += spawnAngAccel; // 逐渐加速旋转
    spawnAngle += spawnAngVel;
    static constexpr float speed = 8.0f;

    BulletPosition pos{ playAreaSettings.logicalWidth / 2.0f, playAreaSettings.logicalHeight / 2.0f };

    // 调高循环次数 (如 100) 用于压力测试或测试处理落机制
    for (int i = 0; i < 1; i++) {
      BulletEmitter::emitRing(pos, spawnAngle, speed, 6);
    }
  }

  // 更新子弹位置, 并回收出界子弹
  m_bulletSystem.update(playAreaSettings.logicalWidth, playAreaSettings.logicalHeight);
}

void World::draw(Graphics::SpriteRenderer* renderer)
{
  m_bulletSystem.draw(renderer, m_textureManager);
}
}
