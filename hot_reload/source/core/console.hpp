#ifndef LIGHTCTRL_BACKEND_CONSOLE_HPP
#define LIGHTCTRL_BACKEND_CONSOLE_HPP

// ============================================================ //
// Headers
// ============================================================ //

#include <iostream>
#include "../core/logger.hpp"

// ============================================================ //
// Class Declaration
// ============================================================ //

class Console {

public:

  Console() = delete;

  ~Console() = delete;

  static Logger& get_logger();

  template <typename ... ARGS>
  static void println(const std::string& format, ARGS&& ... args);

  template <typename ... ARGS>
  static void println(Logger::level level,
                      const std::string& format, ARGS&& ... args);

  static std::string readln() {
    std::string read;
    std::cin >> read;
    return read;
  }

  static void set_level(Logger::level level) { get_logger().set_level(level); };

};

// ============================================================ //
// Class Template Implementation
// ============================================================ //

template <typename ... ARGS>
void Console::println(const std::string& format, ARGS&& ... args) {
  get_logger().log(format, std::forward<ARGS>(args) ...);
}

template <typename ... ARGS>
void Console::println(Logger::level level,
                      const std::string& format, ARGS&& ... args) {
  get_logger().log(level, format, std::forward<ARGS>(args) ...);
}


#endif //LIGHTCTRL_BACKEND_CONSOLE_HPP
