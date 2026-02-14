#include "Timer.hpp"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Core {

Timer::Timer()
  : m_deltaTime(-1.0), m_baseTime(0), m_prevTime(0), m_currTime(0), m_totalTime(0)
{
  std::int64_t countPerSec;
  // 获取高精度计时器频率 (CPU频率)
  QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&countPerSec));
  m_secondsPerCount = 1.0 / static_cast<double>(countPerSec);

  // 初始化时间
  QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_currTime));
  m_baseTime = m_currTime;
  m_prevTime = m_currTime;
}

void Timer::tick()
{
  QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_currTime));
  m_deltaTime = (m_currTime - m_prevTime) * m_secondsPerCount;  // 计算两帧之间的时间差
  m_prevTime = m_currTime; // 准备下一个 tick

  // 防止硬件 bug 等导致的负时间
  if (m_deltaTime < 0.0) {
    m_deltaTime = 0.0;
  }

  // 防止系统休眠或者断点调试导致过大的时间差造成游戏逻辑卡死
  if (m_deltaTime > 0.25) {
    m_deltaTime = 0.25;
  }

  m_totalTime = (m_currTime - m_baseTime) * m_secondsPerCount;
}
}
