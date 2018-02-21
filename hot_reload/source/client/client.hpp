#pragma once

// ============================================================ //
// Headers
// ============================================================ //

#include "../net/tcp_socket.hpp"
#include "../net/tcp_packet.hpp"
#include "../core/buffer.hpp"

// ============================================================ //
// Class Declaration
// ============================================================ //

namespace lightctrl {

  class Client {

  public:

    Client(const std::string& ip, const u16 port);

    void ask();

    void listen();

  private:

    Tcp_socket m_tcp_socket;

  };

}
