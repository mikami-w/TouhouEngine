#pragma once

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

namespace Core {
class Logger
{
public:
  enum class LogLevel : std::uint8_t
  {
    DEBUG_,
    INFO_,
    WARN_,
    // ERROR_FUCKWINDOWS,  // windows.h 里定义了 ERROR 宏 =)
    ERROR_,
    FATAL_,
    DX11_ERROR_
  };

public:
  static Logger& getInstance() noexcept
  {
    static Logger instance;
    return instance;
  }

  void init(char const* logFileName)
  {
#if !defined(_DEBUG)
    std::ofstream(logFileName, std::ios::trunc);
#endif
    m_logFrontBuffer.reserve(128); // 预分配前端缓冲区, 减少动态扩容次数
    m_logFileName = logFileName;
    m_workerThread = std::thread(&Logger::flushLoop, this);
  }

  template <auto VLogLevel>
  void log(std::string_view message)
  {
    std::string logMsg = std::format("{} [{}] {}\n", getCurrentTime(), getEnumName<VLogLevel>(), message);
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_logFrontBuffer.emplace_back(std::move(logMsg));
    }
    m_cv.notify_one();
  }

  void checkDX11(long hr, std::string_view message)
  {
    if (hr < 0) { // FAILED(hr)
      log<LogLevel::DX11_ERROR_>(std::format("{} (HRESULT: 0x{:X})", message, hr));
      throw std::runtime_error(std::format("{} (HRESULT: 0x{:08X})", message, static_cast<std::uint32_t>(hr)));
    }
  }

private:
  Logger() = default;

  ~Logger()
  {
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_exitFlag = true;
    }
    m_cv.notify_all();
    if (m_workerThread.joinable()) {
      m_workerThread.join();
    }
  }

  void flushLoop()
  {
#if defined(_DEBUG)
    std::ostream& logStream = std::cout;
#else
    std::ofstream logStream(m_logFileName, std::ios::app);
#endif

    std::vector<std::string> backBuffer;
    backBuffer.reserve(128); // 这里也要预分配, 不然 swap 后就没了

    for (;;) {
      {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [this] { return !m_logFrontBuffer.empty() || m_exitFlag; });
        if (m_logFrontBuffer.empty() && m_exitFlag) {
          break;
        }
        backBuffer.swap(m_logFrontBuffer);
      }
      for (auto const& logEntry : backBuffer) {
        logStream << logEntry;
      }
      logStream.flush();
      backBuffer.clear();
    }
  }

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

    auto const now = system_clock::now();
    auto const ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000ms;
    std::time_t const now_time_t = system_clock::to_time_t(now);

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

private:
  std::string m_logFileName;
  std::vector<std::string> m_logFrontBuffer;
  std::mutex m_mutex;
  std::condition_variable m_cv;
  std::thread m_workerThread;
  bool m_exitFlag = false;
};
} // namespace Core

#define LOG_INFO(msg) ::Core::Logger::getInstance().log<Core::Logger::LogLevel::INFO_>(msg)
#define LOG_WARN(msg) ::Core::Logger::getInstance().log<Core::Logger::LogLevel::WARN_>(msg)
#define LOG_ERROR(msg) ::Core::Logger::getInstance().log<Core::Logger::LogLevel::ERROR_>(msg)
#define LOG_FATAL(msg) ::Core::Logger::getInstance().log<Core::Logger::LogLevel::FATAL_>(msg)
#define LOG_DX11_CHECK(hr, msg) ::Core::Logger::getInstance().checkDX11(hr, msg)

#if defined(_DEBUG)
#define LOG_DEBUG(msg) ::Core::Logger::getInstance().log<Core::Logger::LogLevel::DEBUG_>(msg)
#else
#define LOG_DEBUG(msg)
#endif
