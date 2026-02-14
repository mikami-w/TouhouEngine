#pragma once

// Standard Library
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <map>
#include <unordered_map>
#include <chrono>
#include <functional>
#include <cstdint>
#include <cstddef>
#include <utility>
#include <string_view>

// DirectX 11
#include <d3d11.h>
#include <d3dcompiler.h>

// Windows API
// Prevent Windows.h from defining min and max macros, and exclude rarely-used services from Windows headers
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
