#include "Timer.hpp"

#include <SDL3/SDL.h>

namespace Core {

Timer::Timer()
  : m_baseTime(SDL_GetPerformanceCounter())
  , m_prevTime(m_baseTime)
  , m_frequency(SDL_GetPerformanceFrequency())
  , m_deltaTime(0.0)
  , m_totalTime(0.0)
{
}

void Timer::tick()
{
  auto currTime = SDL_GetPerformanceCounter();
  m_deltaTime = static_cast<double>(currTime - m_prevTime)
              / static_cast<double>(m_frequency);
  m_prevTime = currTime;

  if (m_deltaTime < 0.0) {
    m_deltaTime = 0.0;
  }
  if (m_deltaTime > 0.25) {
    m_deltaTime = 0.25;
  }

  m_totalTime = static_cast<double>(currTime - m_baseTime)
              / static_cast<double>(m_frequency);
}

double Timer::peekTotalTime() const
{
  auto now = SDL_GetPerformanceCounter();
  return static_cast<double>(now - m_baseTime) / static_cast<double>(m_frequency);
}

}
