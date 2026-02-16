#pragma once

// Standard Library
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <numbers>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

// DirectX 11
#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>

// Windows API
// Prevent Windows.h from defining min and max macros, and exclude rarely-used
// services from Windows headers
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <wrl/client.h>
