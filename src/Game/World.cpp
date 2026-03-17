#include "World.hpp"

#include "BulletEmitter.hpp"
#include "Core/Logger.hpp"
#include "Core/MathUtils.hpp"
#include "Graphics/SpriteRenderer.hpp"
#include "Graphics/TextureManager.hpp"

namespace Game {

World::World(Graphics::TextureManager* textureManager)
  : m_textureManager(textureManager)
  , m_bulletSystem(50000 - 5000)
  , m_entityManager(20000)
{
  BulletEmitter::init(&m_bulletSystem); // 依赖注入, 将 BulletManager 的地址传递给 BulletEmitter
  reset();
  LOG_INFO("World initialized.");
}

void World::reset()
{
  m_logicalFrame = 0;
  m_bulletSystem.clearBullets();
  m_entityManager.clearEntities();
  LOG_INFO("World reset: logical frame reset, bullets and entities cleared.");
}

void World::update(Core::Config::PlayAreaSettings& playAreaSettings)
{
  namespace Math = Core::Math;
  ++m_logicalFrame;

  // 只执行一次, 在游戏区域边界生成一圈静止子弹; 生成中心发弹实体
  static int const blackMagicHack_initSpawn = [&] {
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

    Core::Position epos{ playAreaSettings.logicalWidth / 2.0f, playAreaSettings.logicalHeight / 2.0f };
    m_entityManager.spawnEntity(Entity(epos)); // 需要接收值

    return 0;
  }();

  {
    Core::Position pos{ playAreaSettings.logicalWidth / 2.0f, playAreaSettings.logicalHeight / 2.0f };

    // 生成子弹
    static constexpr double spawnAngAccel = 0.001f;
    static double spawnAngle = 0.0f;
    static double spawnAngVel = 0.0f;
    spawnAngVel += spawnAngAccel; // 逐渐加速旋转
    spawnAngle += spawnAngVel;
    static constexpr float speed = 8.0f;

    // 调高循环次数 (如 100) 用于压力测试或测试处理落机制
    for (int i = 0; i < 1; i++) {
      BulletEmitter::emitRing(pos, spawnAngle, speed, 6);
    }
  }

  // 更新子弹位置, 并回收出界子弹
  m_bulletSystem.update(playAreaSettings.logicalWidth, playAreaSettings.logicalHeight);
  m_entityManager.update(playAreaSettings.logicalWidth, playAreaSettings.logicalHeight);
}

void World::draw(Graphics::SpriteRenderer* renderer) const
{
  m_bulletSystem.draw(renderer, m_textureManager);
  m_entityManager.draw(renderer, m_textureManager);
}
}
