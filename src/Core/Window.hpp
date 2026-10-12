#pragma once

#include <string>

#include <SDL3/SDL.h>

namespace Core {
class Window
{
public:
  struct Config
  {
    std::string title;
    int width;
    int height;
  };

public:
  explicit Window(Config const& config);
  ~Window();

  Window(Window const&) = delete;
  Window& operator=(Window const&) = delete;

  SDL_Window* getHandle() const { return m_window; }

private:
  SDL_Window* m_window = nullptr;
};
} // namespace Core
