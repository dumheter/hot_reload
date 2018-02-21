#ifndef LIGHTCTRL_BACKEND_SERVER_HPP
#define LIGHTCTRL_BACKEND_SERVER_HPP

// ============================================================ //
// Headers
// ============================================================ //

#include "../core/console.hpp"
#include "../core/logger.hpp"
#include "../net/tcp_socket.hpp"
#include "../net/tcp_packet.hpp"
#include "../core/buffer.hpp"
#include <vector>

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

    int answer_question(const std::string& question);

  private:

    Tcp_socket m_socket{};
    uint16_t m_port;
    std::vector<Tcp_socket> m_clients;

  };

}

#endif //LIGHTCTRL_BACKEND_SERVER_HPP
