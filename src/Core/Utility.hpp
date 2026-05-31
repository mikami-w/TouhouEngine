#pragma once

#include <ctime>
#include <string_view>

#include "Platform.hpp"

// 跨平台 localtime
inline void LocaltimeP(std::tm* out, std::time_t const* timer)
{
  if (!out || !timer) {
    return;
  }

#if defined(_WIN32) || defined(_MSC_VER)
  // Windows 平台 (MSVC / MinGW 某些配置)
  (void)::localtime_s(out, timer);
#elif defined(__unix__) || defined(__linux__) || defined(__APPLE__) || defined(__clang__) || defined(__GNUC__)
  // POSIX 平台 (Linux, macOS)
  (void)::localtime_r(timer, out);
#else
#pragma message("Warning: Using non-thread-safe std::localtime as fallback.")
  if (std::tm* res = std::localtime(timer)) {
    *out = *res;
  }
#endif
}

template <auto V>
constexpr std::string_view GetFunctionSig() noexcept
{
#if defined(_MSC_VER)
  return __FUNCSIG__;
#elif defined(__GNUC__) || defined(__clang__)
  return __PRETTY_FUNCTION__;
#else
  return "unsupported compiler for function \"GetFunctionSig\"";
#endif
}

template <auto V>
constexpr std::string_view GetEnumName() noexcept
{
  constexpr std::string_view sig = GetFunctionSig<V>();
#if defined(_MSC_VER)
  constexpr std::string_view prefix = "GetFunctionSig<";
  constexpr std::string_view suffix = ">(void) noexcept";
#elif defined(__clang__)
  // Clang: "std::string_view GetFunctionSig() [V = TestEnum::Value]"
  constexpr std::string_view prefix = "[V = ";
  constexpr std::string_view suffix = "]";
#elif defined(__GNUC__)
  // GCC: "constexpr std::string_view GetFunctionSig() [with auto V = TestEnum::Value; std::string_view = std::basic_string_view<char>]"
  constexpr std::string_view prefix = "[with auto V = ";
  constexpr std::string_view suffix = "; std::string_view = std::basic_string_view<char>]";
#else
  return "unsupported compiler for function \"GetEnumName\"";
#endif
  std::size_t start = sig.find(prefix) + prefix.size();
  std::size_t end = sig.rfind(suffix);
  return sig.substr(start, end - start);
}
