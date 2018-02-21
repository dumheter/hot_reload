// ============================================================ //
// Headers
// ============================================================ //

#include "server.hpp"
#include <exception>

// ============================================================ //
// Class Implementation
// ============================================================ //

namespace lightctrl {

  Server::Server(uint16_t port) : m_port(port) {
    m_socket.open();
    m_socket.set_reuse_addr(true);
    m_socket.bind(m_port);
    m_socket.listen();

    Console::println(Logger::level::info,
                     "Server up and listening on port {}.", port);
  }

  // ============================================================ //

  void Server::run() {
    accept_connections();
    read();
  }

  // ============================================================ //

  void Server::read() {
    bool remove_closed_clients = false;

    for (auto& client : m_clients) {
      if (client.can_read()) {
        try {
          Tcp_packet packet(Tcp_packet::Packet_signature::INVALID, 1024);
          client.read(packet.get_buffer());
          Console::println(Logger::level::warn, "server: read: {} | len: {}, sig: {}",
            packet.get_payload_as_string(),
            packet.get_buffer().size(),
            packet.get_signature_as_string()
          );

          const int answer = answer_question(packet.get_payload_as_string());

          packet.set_signature(Tcp_packet::Packet_signature::RESPONSE);
          const Buffer<u8> buffer(std::to_string(answer));
          packet.set_payload(buffer);
          const ssize_t sent_bytes = client.write(packet.get_buffer());
          Console::println("server: answering {}, sent_bytes: {}", 
            packet.get_payload_as_string(),
            sent_bytes
          );
        }
        catch (socket_exception&) {
          try {
            client.close();
          }
          catch (socket_exception&) {};
          remove_closed_clients = true;
        }
      }
    }

    if (remove_closed_clients)
      purge_clients();
  }

  // ============================================================ //

  void Server::accept_connections() {
    if (m_socket.can_accept()) {
      Console::println("accepted connection");
      m_clients.emplace_back(m_socket.accept());
      std::string client_address = m_clients.back().get_address();
      Console::println("Client connected from {}.", client_address);
    }
  }

  void Server::purge_clients() {
    const auto size_before = m_clients.size();
    m_clients.erase(std::remove_if(m_clients.begin(), m_clients.end(),
      [](auto& client) {
      return !client.is_valid();
    }), m_clients.end());

    if (size_before > m_clients.size()) {
      Console::println(Logger::level::info,
        "Client disconnected | There are {} connected devices.",
        m_clients.size());
    }
  }

  int Server::answer_question(const std::string& question) {
    std::string num1, num2;

    const unsigned long long comma = question.find(',');
    if (comma == std::string::npos)
      throw std::runtime_error("failed to find comma");

    num1 = question.substr(0, comma);
    num2 = question.substr(comma+1);

    return std::stoi(num1) + std::stoi(num2);
  }

}
