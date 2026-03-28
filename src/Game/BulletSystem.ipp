namespace Game::BulletSystem {

inline BulletView GetBullet(BulletHandle handle)
{
  return detail::g_bulletRouter->getBullet(handle);
}

inline void CancelBullet(BulletHandle handle, bool spawnBonus) noexcept
{
  detail::g_bulletRouter->cancelBullet(handle, spawnBonus);
}

template <SimpleMotion BulletMotion>
void SetMotionModule(BulletHandle& handle, BulletMotion newMotion) noexcept
{
  detail::g_bulletRouter->setMotionModule(handle, newMotion);
}

inline void SetPosition(BulletHandle handle, Position pos) noexcept
{
  *GetBullet(handle).position = pos;
}

inline void SetMetadata(BulletHandle handle, BulletMetadata meta) noexcept
{
  *GetBullet(handle).metadata = meta;
}

inline void ClearBullets() noexcept
{
  detail::g_bulletRouter->clearBullets();
}
} // namespace Game::BulletSystem
