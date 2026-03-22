namespace Game::BulletSystem {

inline BulletView getBullet(BulletHandle handle)
{
  return detail::g_bulletRouter->getBullet(handle);
}

inline void cancelBullet(BulletHandle handle, bool spawnBonus) noexcept
{
  detail::g_bulletRouter->cancelBullet(handle, spawnBonus);
}

template <SimpleMotion BulletMotion>
void setMotionModule(BulletHandle& handle, BulletMotion newMotion) noexcept
{
  detail::g_bulletRouter->setMotionModule(handle, newMotion);
}

inline void setPosition(BulletHandle handle, Position pos) noexcept
{
  *getBullet(handle).position = pos;
}

inline void setMetadata(BulletHandle handle, BulletMetadata meta) noexcept
{
  *getBullet(handle).metadata = meta;
}

inline void clearBullets() noexcept
{
  detail::g_bulletRouter->clearBullets();
}
} // namespace Game::BulletSystem
