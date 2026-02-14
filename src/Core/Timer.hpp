#pragma once

#include <cstdint>

namespace Core {
class Timer
{
public:
  Timer();

  // 记录上一帧到这一帧经过的时间 (s)
  void tick();

  // 获取两帧之间的增量时间 (Delta Time, s)
  double getDeltaTime() const { return m_deltaTime; }

  // 获取从启动到现在的总时间 (Total Time, s)
  double getTotalTime() const { return m_totalTime; }

private:
  double m_secondsPerCount; // 计数器每个 tick 代表多少秒
  std::int64_t m_baseTime;  // 启动时间
  std::int64_t m_prevTime;  // 上一帧时间
  std::int64_t m_currTime;  // 当前帧时间

  double m_deltaTime;
  double m_totalTime;
};
}
