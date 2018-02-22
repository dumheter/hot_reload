// ============================================================ //
// Headers
// ============================================================ //

#include "console.hpp"

// ============================================================ //
// Class Implementation
// ============================================================ //

bool Console::m_write_to_file = true;

std::string Console::m_name = "console";

Logger& Console::get_logger () {
  static Logger logger(m_name, m_write_to_file);
  return logger;
}
