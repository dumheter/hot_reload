#ifndef LIGHTCTRL_BACKEND_LOGGER_HPP
#define LIGHTCTRL_BACKEND_LOGGER_HPP

// ============================================================ //
// Headers
// ============================================================ //

#include "../thirdparty/spdlog/spdlog.h"

// ============================================================ //
// Class Declaration
// ============================================================ //

class Logger {

public:

  enum class level {
    trace,
      debug,
      info,
      warn,
      err,
      critical,
      off
      };

  explicit Logger(const std::string& name);

  template <typename ... ARGS>
  void log(const std::string& format, ARGS&& ... args) const;

  template <typename ... ARGS>
  void log(Logger::level level,
           const std::string& format, ARGS&& ... args) const;

  void set_level(Logger::level level) const;

private:

  std::string _name;

  std::shared_ptr<spdlog::logger> _logger;

};


// ============================================================ //
// Class Template Implementation
// ============================================================ //

template <typename ... ARGS>
void Logger::log(const std::string& format, ARGS&& ... args) const {
  _logger->debug(format.c_str(), std::forward<ARGS>(args) ...);
}

template <typename ... ARGS>
void Logger::log(Logger::level level,
                 const std::string& format, ARGS&& ... args) const {
  spdlog::level::level_enum spdlog_level;

  switch (level) {

  case Logger::level::trace:
    spdlog_level = spdlog::level::trace;
    break;

  case Logger::level::debug:
    spdlog_level = spdlog::level::debug;
    break;

  case Logger::level::info:
    spdlog_level = spdlog::level::info;
    break;

  case Logger::level::warn:
    spdlog_level = spdlog::level::warn;
    break;

  case Logger::level::err:
    spdlog_level = spdlog::level::err;
    break;

  case Logger::level::critical:
    spdlog_level = spdlog::level::critical;
    break;

  case Logger::level::off:
    spdlog_level = spdlog::level::off;
    break;

  default:
    spdlog_level = spdlog::level::debug;
  }

  if (spdlog_level != spdlog::level::off) {
    _logger->log(spdlog_level, format.c_str(), std::forward<ARGS>(args) ...);
  }
}

#endif //LIGHTCTRL_BACKEND_LOGGER_HPP
