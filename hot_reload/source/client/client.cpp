
// ============================================================ //
// Headers
// ============================================================ //

#include "client.hpp"
#include "../core/console.hpp"

// ============================================================ //
// Class Implementation
// ============================================================ //

namespace lightctrl {


  Client::Client(const std::string& ip, const u16 port) {
    m_tcp_socket.open();
    m_tcp_socket.connect(ip, port);
  }

  void Client::ask() {
    const std::string question = "What is 1+1";
    const Buffer<u8> buffer(question);
    Tcp_packet packet(Tcp_packet::Packet_signature::REQUEST, buffer);
    m_tcp_socket.write(packet.get_buffer());

    Console::println("Asked: {}", question);
  }

  void Client::listen() {
    while (!m_tcp_socket.can_read());

    Tcp_packet packet(Tcp_packet::Packet_signature::INVALID, 1024);
    m_tcp_socket.read(packet.get_buffer());
    
    Console::println("Got answer {}.", packet.get_payload_as_string());
  }
}
