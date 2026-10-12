#pragma once

#include <cstdint>

namespace Core {
class Timer
{
public:
  Timer();

  void tick();

  double getDeltaTime() const { return m_deltaTime; }
  double getTotalTime() const { return m_totalTime; }

  // 只读查询, 不修改 deltaTime (用于自旋等待)
  double peekTotalTime() const;

private:
  std::uint64_t m_baseTime;
  std::uint64_t m_prevTime;
  std::uint64_t m_frequency;

  double m_deltaTime;
  double m_totalTime;
};
}
