#include "Window.hpp"

#include <stdexcept>

#include "Logger.hpp"

namespace Core {
Window::Window(Config const& config)
{
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    LOG_ERROR(std::format("SDL_Init failed: {}", SDL_GetError()));
    throw std::runtime_error("SDL_Init failed");
  }

  m_window = SDL_CreateWindow(
    config.title.c_str(),
    config.width,
    config.height,
    SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
  );

  if (!m_window) {
    LOG_ERROR(std::format("SDL_CreateWindow failed: {}", SDL_GetError()));
    throw std::runtime_error("SDL_CreateWindow failed");
  }

  LOG_INFO("SDL3 window created successfully.");
}

Window::~Window()
{
  if (m_window) {
    SDL_DestroyWindow(m_window);
  }
  SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

} // namespace Core
