#include "World.hpp"

#include "BulletEmitter.hpp"
#include "BulletSystem.hpp"
#include "Core/Logger.hpp"
#include "Core/MathUtils.hpp"
#include "Core/XSRandom.hpp"
#include "Graphics/SpriteRenderer.hpp"
#include "Graphics/TextureManager.hpp"

namespace Game {

World::World(Graphics::TextureManager* textureManager)
  : m_textureManager(textureManager)
  , m_bulletManager(50000 - 5000)
  , m_entityManager(20000)
{
  BulletEmitter::detail::Init(&m_bulletManager); // 依赖注入, 将 BulletRouter 的地址传递给 BulletEmitter
  BulletSystem::detail::Init(&m_bulletManager);  // 依赖注入, 将 BulletRouter 的地址传递给 BulletSystem
  reset();
  LOG_INFO("World initialized.");
}

void World::reset()
{
  m_logicalFrame = 0;
  m_bulletManager.clearBullets();
  m_entityManager.clearEntities();
  LOG_INFO("World reset: logical frame reset, bullets and entities cleared.");
}

void World::update(Core::Config::PlayAreaSettings& playAreaSettings)
{
  ++m_logicalFrame;

  _temp_logic_to_be_replaced(playAreaSettings); // 临时代码, 用于测试和演示, 之后会被正式游戏逻辑替换

  // 更新子弹位置, 并回收出界子弹
  m_bulletManager.update(playAreaSettings.logicalWidth, playAreaSettings.logicalHeight);
  m_entityManager.update(playAreaSettings.logicalWidth, playAreaSettings.logicalHeight);
}

void World::draw(Graphics::SpriteRenderer* renderer) const
{
  m_bulletManager.draw(renderer, m_textureManager);
  m_entityManager.draw(renderer, m_textureManager);
}

void World::_temp_logic_to_be_replaced(Core::Config::PlayAreaSettings& playAreaSettings)
{
  namespace Math = Core::Math;

  Core::Vector2 center{ playAreaSettings.logicalWidth / 2.0f, playAreaSettings.logicalHeight / 2.0f };
  // 只执行一次, 在游戏区域边界生成一圈静止子弹; 生成中心发弹实体
  static auto const blackMagicHack_initSpawn_andReturnHandle = [&] {
    Core::Vector2 pos{};
    while (pos.y <= playAreaSettings.logicalHeight) {
      m_bulletManager.spawnBullet(pos, MotionModule{}, {});
      pos.x = playAreaSettings.logicalWidth;
      m_bulletManager.spawnBullet(pos, MotionModule{}, {});
      pos.y += 10.0f;
      pos.x = 0.0f;
    }
    pos = {};
    while (pos.x <= playAreaSettings.logicalWidth) {
      m_bulletManager.spawnBullet(pos, MotionModule{}, {});
      pos.y = playAreaSettings.logicalHeight;
      m_bulletManager.spawnBullet(pos, MotionModule{}, {});
      pos.x += 10.0f;
      pos.y = 0.0f;
    }

    auto handle = m_entityManager.spawnEntity(Entity(center)); // 需要接收值

    return handle;
  }();

  if (0) {
    // 实体句柄测试
    auto& entityHandle = blackMagicHack_initSpawn_andReturnHandle;
    if (auto e = m_entityManager.getEntity(entityHandle)) {
      // LOG_INFO("Entity spawned at position: (" + std::to_string(entity.getPosition().x) + ", " +
      //          std::to_string(entity.getPosition().y) + ")");
      e->setPosition({ center.x + Math::sin(m_logicalFrame * 0.1) * 50, center.y }); // 可以修改实体位置
    } else {
      LOG_ERROR("Invalid entity handle.");
    }
  }

  if (1) {
    // 生成子弹测试-波与粒的境界
    static constexpr double spawnAngAccel = 0.001f;
    static double spawnAngle = 0.0f;
    static double spawnAngVel = 0.0f;
    spawnAngVel += spawnAngAccel; // 逐渐加速旋转
    spawnAngle += spawnAngVel;
    static constexpr float speed = 8.0f;

    MotionModule firstMotionModule;
    firstMotionModule.polar.angle = spawnAngle;
    firstMotionModule.polar.speed = speed;

    // 调高循环次数 (如 100) 用于压力测试或测试处理落机制
    for (int i = 0; i < 1; i++) {
      BulletEmitter::EmitFan(center, firstMotionModule, {}, 3, 50);
    }
  }

  if (1) {
    // 生成子弹测试-运动叠加
    static Math::XSRandom rand(42); // 随机数生成器
    if (m_logicalFrame % 13 == 0) {
      Core::Vector2 emitPoint = center;
      emitPoint.x = center.x + rand.rangedFloat(-50.0f, 50.0f); // 在中心附近随机水平位置发射
      emitPoint.y = center.y + rand.rangedFloat(-50.0f, 50.0f);
      MotionModule motionModule{
        .polar = { .angle = 0.0f, .angVel = 0.01f, .angAccel = 0.0f, .speed = 1.0f, .tanAccel = 0.0f },
        .rect = { 3.0f * (-1.0f + rand.nextBoolean() * 2), 0.0f, 0.0f, 0.1f }
      };

      BulletEmitter::EmitRing(center, motionModule, {}, 16, 100.0f);
    }
  }
}
}
