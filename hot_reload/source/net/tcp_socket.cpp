// ============================================================ //
// Headers
// ============================================================ //

#include "tcp_socket.hpp"

// ============================================================ //
// Class Implementation
// ============================================================ //

namespace lightctrl {

  void Tcp_socket::open() {
    auto res = chif_net_open_socket(&m_socket, CHIF_PROTOCOL_TCP,
                                    CHIF_ADDRESS_FAMILY_IPV4);
    if (res != CHIF_RESULT_SUCCESS) {
      throw socket_exception("failed to open socket");
    }
  }

  // ============================================================ //

  void Tcp_socket::close() {
    auto res = chif_net_close_socket(&m_socket);

    if (res != CHIF_RESULT_SUCCESS) {
      throw socket_exception("failed to close socket");
    }
  }

  // ============================================================ //

  ssize_t Tcp_socket::read(uint8_t *buffer, size_t size) {
    ssize_t read_bytes;
    auto res = chif_net_read(m_socket, buffer, size, &read_bytes);

    if (res != CHIF_RESULT_SUCCESS) {
      if (res == CHIF_RESULT_CONNECTION_CLOSED)
        throw socket_exception("failed to read socket, connection closed");
      else
        throw socket_exception("failed to read socket, unknown error");
    }

    return read_bytes;
  }

  // ============================================================ //

  void Tcp_socket::read(Buffer<u8> &buffer) {
    const auto size = read(buffer.raw(), static_cast<size_t>(buffer.capacity()));
    buffer.set_size(size);
  }

  // ============================================================ //

  ssize_t Tcp_socket::write(const uint8_t *buffer, size_t size) {
    ssize_t sent_bytes;
    auto res = chif_net_write(m_socket, buffer, size, &sent_bytes);

    if (res != CHIF_RESULT_SUCCESS) {
      throw socket_exception("failed to write to socket");
    }

    return sent_bytes;
  }

  // ============================================================ //

  ssize_t Tcp_socket::write(const Buffer<u8> &buffer) {
    const auto wrote = write(buffer.raw(), static_cast<size_t>(buffer.size()));
    if ((wrote < 0 ? static_cast<u64>(0) : static_cast<u64>(wrote)) != buffer.size()) {
      //TODO this...
      throw std::runtime_error("Did not write all data. TODO handle that");
    }
    return wrote;
  }

  // ============================================================ //

  void Tcp_socket::connect(chif_net_address &address) {
    auto res = chif_net_connect(m_socket, address);

    if (res != CHIF_RESULT_SUCCESS) {
      throw socket_exception("failed to connect to remote");
    }
  }

  // ============================================================ //

  void Tcp_socket::connect(const std::string &ip_address, u16 port) {
    chif_net_address address{};
    auto res = chif_net_create_address(&address, ip_address.c_str(), port,
                                       CHIF_ADDRESS_FAMILY_IPV4);

    if (res != CHIF_RESULT_SUCCESS) {
      throw socket_exception("failed to connect, bad address");
    }

    res = chif_net_connect(m_socket, address);

    if (res != CHIF_RESULT_SUCCESS) {
      throw socket_exception("failed to connect");
    }
  }

  // ============================================================ //

  void Tcp_socket::bind(u16 port) {
    const auto res = chif_net_bind(m_socket, port, CHIF_ADDRESS_FAMILY_IPV4);

    if (res != CHIF_RESULT_SUCCESS) {
      throw socket_exception("failed to bind");
    }
  }

  // ============================================================ //

  void Tcp_socket::listen() {
    const auto res = chif_net_listen(m_socket, CHIF_DEFAULT_MAXIMUM_BACKLOG);

    if (res != CHIF_RESULT_SUCCESS) {
      throw socket_exception("failed to listen");
    }
  }

  // ============================================================ //

  Tcp_socket Tcp_socket::accept() {
    Tcp_socket client_socket;
    chif_net_address client_address{};

    const auto socket = chif_net_accept(m_socket, &client_address);

    if (socket == CHIF_SOCKET_ERROR)
      throw socket_exception("failed to accept");

    client_socket.set_socket(socket);

    return client_socket;
  }

  // ============================================================ //

  bool Tcp_socket::can_read() {
    chif_bool socket_can_read;
    auto res = chif_net_can_read(m_socket, &socket_can_read);

    if (res != CHIF_RESULT_SUCCESS)
      throw socket_exception("failed to check if socket can read");

    return socket_can_read == CHIF_TRUE;
  }

  // ============================================================ //

  bool Tcp_socket::can_accept() {
    return can_read();
  }

  // ============================================================ //

  bool Tcp_socket::has_error() {
    chif_bool socket_has_error;
    auto res = chif_net_has_error(m_socket, &socket_has_error);

    if (res != CHIF_RESULT_SUCCESS)
      throw socket_exception("failed to check if socket has error");

    return socket_has_error == CHIF_TRUE;
  }

  // ============================================================ //

  std::string Tcp_socket::get_address() {
    char address[CHIF_IPVX_STRING_LENGTH]{};
    uint16_t port;

    chif_net_get_peer_name(m_socket, address, CHIF_IPVX_STRING_LENGTH);
    chif_net_get_peer_port(m_socket, &port);

    size_t address_len = CHIF_IPVX_STRING_LENGTH;
    for (size_t i=0; i<CHIF_IPVX_STRING_LENGTH; i++) {
      if (address[i] == 0) {
        address_len = i;
        break;
      }
    }

    const char* tmp = address;
    std::string s_address = std::string(tmp, address_len) + ":" + std::to_string(port);

    return s_address;
  }

  void Tcp_socket::set_reuse_addr(bool reuse) {
    chif_net_set_reuse_addr(m_socket, static_cast<chif_bool>(reuse));
  }

}
