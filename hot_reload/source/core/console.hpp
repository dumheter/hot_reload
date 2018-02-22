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

  static void set_write_to_file(const bool write_to_file) { m_write_to_file = write_to_file; };

  /**
   * WARNING
   * as of now, the name identifies the console. If you change the name, a new console will be created.
   */
  static void set_name(const std::string& name) { m_name = name; };

private:

  static bool m_write_to_file;

  static std::string m_name;
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
