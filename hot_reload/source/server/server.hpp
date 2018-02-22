#ifndef LIGHTCTRL_BACKEND_SERVER_HPP
#define LIGHTCTRL_BACKEND_SERVER_HPP
#pragma once 

// ============================================================ //
// Headers
// ============================================================ //

#include "../core/console.hpp"
#include "../core/logger.hpp"
#include "../net/tcp_socket.hpp"
#include "../net/tcp_packet.hpp"
#include "../core/buffer.hpp"
#include <vector>
#define CR_HOST CR_UNSAFE
#include "../thirdparty/cr/cr.h"


// ============================================================ //
// Data types
// ============================================================ //

struct Host_data {
  int a = 0;
  int b = 0;
  int result = 0;
};

// ============================================================ //
// Class Declaration
// ============================================================ //

namespace lightctrl {

  class Server {

  public:

    /**
     * Open a socket, bind it to port and start listening.
     *
     * On error it will exit the program with critical log message.
     */
    explicit Server(uint16_t port);

    ~Server();

    /**
     * Main loop
     */
    void run();

    /**
     * Read incoming traffic if there is any
     */
    void read();

    /**
     * Accept new tcp connections if there are any.
     */
    void accept_connections();

    void purge_clients();

  private:

    Tcp_socket m_socket{};
    uint16_t m_port;
    std::vector<Tcp_socket> m_clients;

    cr_plugin m_ctx;
    const char* quick_maths_dll_path = "C:/Users/chris/Documents/github/hot_reload/x64/Debug/quick_maths.dll";
    Host_data m_ctx_data{};

  };

}

#endif //LIGHTCTRL_BACKEND_SERVER_HPP
