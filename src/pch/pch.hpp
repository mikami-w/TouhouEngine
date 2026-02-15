#pragma once

// Standard Library
#include <map>
#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <utility>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <filesystem>
#include <functional>
#include <string_view>
#include <unordered_map>

// DirectX 11
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

// Windows API
// Prevent Windows.h from defining min and max macros, and exclude rarely-used services from Windows headers
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <wrl/client.h>
