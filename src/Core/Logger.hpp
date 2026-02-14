#pragma once

#include <iostream>
#include <cstdint>
#include <string_view>
#include <chrono>

namespace Core {
class Logger
{
public:
  enum class LogLevel: std::uint8_t
  {
    DEBUG_,
    INFO_,
    WARN_,
    // ERROR_FUCKWINDOWS,  // windows.h 里定义了 ERROR 宏 =)
    ERROR_,
    FATAL_,
    DX11_FATAL_
  };

  template <auto VLogLevel>
  static void log(std::string_view message, std::ostream& os = std::cout)
  {
    os << std::format("{} [{}] {}\n", getCurrentTime(), getEnumName<VLogLevel>(), message);
  }

  static void checkDX11(long hr, std::string_view message)
  {
    if (hr < 0) { // FAILED(hr)
      log<LogLevel::DX11_FATAL_>(std::format("{} (HRESULT: 0x{:X})", message, hr));
      throw std::runtime_error(std::string(message) + " (HRESULT: " + std::to_string(hr) + ")");
    }
  }

private:
  template <auto V>
  static constexpr std::string_view getEnumName() noexcept
  {
    constexpr std::string_view sig = __FUNCSIG__;
    constexpr std::string_view prefix = "<Core::Logger::LogLevel::";
    constexpr std::string_view suffix = "_>(void) noexcept";

    std::size_t start = sig.find(prefix) + prefix.size();
    std::size_t end = sig.rfind(suffix);

    return sig.substr(start, end - start);
  }

  static std::string getCurrentTime() noexcept
  {
    using namespace std::chrono;

    const auto now = system_clock::now();
    const auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000ms;
    const std::time_t now_time_t = system_clock::to_time_t(now);

    std::tm now_tm{};
    localtime_s(&now_tm, &now_time_t);

    return std::format("{:04}-{:02}-{:02} {:02}:{:02}:{:02}.{:03}",
                       now_tm.tm_year + 1900,
                       now_tm.tm_mon + 1,
                       now_tm.tm_mday,
                       now_tm.tm_hour,
                       now_tm.tm_min,
                       now_tm.tm_sec,
                       ms.count());
  }
};
} // namespace Core

#define LOG_DEBUG(msg) ::Core::Logger::log<Core::Logger::LogLevel::DEBUG_>(msg)
#define LOG_INFO(msg) ::Core::Logger::log<Core::Logger::LogLevel::INFO_>(msg)
#define LOG_WARN(msg) ::Core::Logger::log<Core::Logger::LogLevel::WARN_>(msg)
#define LOG_ERROR(msg) ::Core::Logger::log<Core::Logger::LogLevel::ERROR_>(msg)
#define LOG_FATAL(msg) ::Core::Logger::log<Core::Logger::LogLevel::FATAL_>(msg)
#define LOG_DX11_CHECK(hr, msg) ::Core::Logger::checkDX11(hr, msg)
