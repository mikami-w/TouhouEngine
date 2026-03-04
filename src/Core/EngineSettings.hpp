#pragma once

#include <cstddef>
#include <cstdint>

namespace Core::Config {
struct WindowSettings
{
  char title[256] = "Touhou Engine";
  int width = 1280;
  int height = 960;
  bool vsync = false;
  bool fullscreen = false;
};

struct PlayAreaSettings
{
  float logicalWidth = 384.0f;
  float logicalHeight = 448.0f;
  float offsetX = 64.0f;
  float offsetY = 32.0f;
  float scale = 2.0f;
};

struct DebugSettings
{
  bool showFPS = true;
  bool showActiveBulletCount = true;
  bool showHitbox = false;
};

struct EngineSettings
{
  WindowSettings window{};
  PlayAreaSettings playArea{};
  DebugSettings debug{};
};
} // namespace Core::Config
