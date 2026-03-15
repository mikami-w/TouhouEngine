#include "World.hpp"

#include "Core/Logger.hpp"
#include "Core/MathUtils.hpp"
#include "Graphics/SpriteRenderer.hpp"
#include "Graphics/TextureManager.hpp"

namespace Game {

World::World(Graphics::TextureManager* textureManager)
  : m_textureManager(textureManager)
{
  m_bulletManager.init(50000 - 5000); // 初始化弹幕池, 最多支持的子弹数取决于 SpriteRenderer 的实例缓冲区大小 (50000)
  init();
  LOG_INFO("World initialized with BulletManager capacity: " + std::to_string(50000 - 5000));
}

void World::init()
{
  m_logicalFrame = 0;
  m_bulletManager.clearBullets();
  LOG_INFO("World init: logical frame reset and bullets cleared.");
}

void World::update(Core::Config::PlayAreaSettings& playAreaSettings)
{
  namespace Math = Core::Math;
  ++m_logicalFrame;

  // 只执行一次, 在游戏区域边界生成一圈静止子弹
  static int const blackMagicHack_spawnEdge = [&] {
    Bullet e{};
    while (e.pos.y <= playAreaSettings.logicalHeight) {
      m_bulletManager.spawnBullet(e);
      e.pos.x = playAreaSettings.logicalWidth;
      m_bulletManager.spawnBullet(e);
      e.pos.y += 10.0f;
      e.pos.x = 0.0f;
    }
    e = {};
    while (e.pos.x <= playAreaSettings.logicalWidth) {
      m_bulletManager.spawnBullet(e);
      e.pos.y = playAreaSettings.logicalHeight;
      m_bulletManager.spawnBullet(e);
      e.pos.x += 10.0f;
      e.pos.y = 0.0f;
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
    Bullet b{ .pos = { playAreaSettings.logicalWidth / 2.0f, playAreaSettings.logicalHeight / 2.0f } };
    b.angle = spawnAngle;
    b.speed = 8.0f;

    // 调高循环次数 (如 100) 用于压力测试或测试处理落机制
    for (int i = 0; i < 1; i++) {
      // 每帧生成 3 颗子弹
      m_bulletManager.spawnBullet(b);
      b.angle += Math::PI2_3_f;
      m_bulletManager.spawnBullet(b);
      b.angle += Math::PI2_3_f;
      m_bulletManager.spawnBullet(b);
    }
  }

  // 更新子弹位置, 并回收出界子弹
  m_bulletManager.update(playAreaSettings.logicalWidth, playAreaSettings.logicalHeight);
}

void World::draw(Graphics::SpriteRenderer* renderer)
{
  namespace Math = Core::Math;

  Bullet const* bullets = m_bulletManager.getActiveBullets();
  std::size_t count = m_bulletManager.getActiveCount();

  // 暂时复用八云紫的贴图来当做子弹, 缩小到 20x20
  auto yukari = m_textureManager->getTexture("yukari");
  for (std::size_t i = 0; i < count; i++) {
    Bullet const& b = bullets[i];
    renderer->drawSprite(yukari,
                         b.pos,
                         b.angle - Math::PI_2_f, // 子弹总是面向运动方向
                         { 20.0f, 20.0f }        // 子弹大小
    );
  }
}
}
