// ============================================================ //
// Headers
// ============================================================ //

#include "../core/logger.hpp"
#include "../core/platform.hpp"

#include <vector>
#include <iostream>

// ============================================================ //
// Class Implementation
// ============================================================ //

Logger::Logger(const std::string& name, const bool write_to_file) {
  std::vector<spdlog::sink_ptr> sinks;

#if defined(LIGHTCTRL_PLATFORM_WINDOWS)
  sinks.push_back(std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>());
#elif defined(LIGHTCTRL_PLATFORM_LINUX)
  sinks.push_back(std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>());
#else
  static_assert(false, "logger not implemented for this platform");
#endif

  if (write_to_file) {
    sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/" + name + ".txt", 23, 59));
  }

  _logger = std::make_shared<spdlog::logger>(name, begin(sinks), end(sinks));
  spdlog::register_logger(_logger);
  _logger->set_level(spdlog::level::trace);
  _logger->flush_on(spdlog::level::critical);
}

void Logger::set_level(Logger::level level) const {
  switch (level) {
  case Logger::level::trace:
    _logger->set_level(spdlog::level::trace);
    break;

  case Logger::level::off:
    _logger->set_level(spdlog::level::off);
    break;

  case Logger::level::info:
    _logger->set_level(spdlog::level::info);
    break;

  case Logger::level::warn:
    _logger->set_level(spdlog::level::warn);
    break;

  case Logger::level::critical:
    _logger->set_level(spdlog::level::critical);
    break;

  case Logger::level::err:
    _logger->set_level(spdlog::level::err);
    break;

  default:
    _logger->set_level(spdlog::level::debug);
  }
}
