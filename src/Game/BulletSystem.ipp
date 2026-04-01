namespace Game::BulletSystem {

inline void CancelBullet(BulletManager::Handle handle, bool spawnBonus) noexcept
{
  detail::g_bulletManager->cancelBullet(handle, spawnBonus);
}

inline void SetMotionModule(BulletManager::Handle handle, MotionModule newMotion) noexcept
{
  detail::g_bulletManager->setMotionModule(handle, newMotion);
}

inline void SetPosition(BulletManager::Handle handle, Core::Vector2 pos) noexcept
{
  detail::g_bulletManager->setPosition(handle, pos);
}

inline void SetMetadata(BulletManager::Handle handle, BulletMetadata meta) noexcept
{
  detail::g_bulletManager->setMetadata(handle, meta);
}

inline void ClearBullets() noexcept
{
  detail::g_bulletManager->clearBullets();
}
} // namespace Game::BulletSystem
