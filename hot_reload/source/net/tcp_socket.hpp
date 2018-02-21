/**
 * Notes:
 * Should TCPSocket hold information about address?
 */

#ifndef LIGHTCTRL_BACKEND_TCP_SOCKET_HPP
#define LIGHTCTRL_BACKEND_TCP_SOCKET_HPP

// ============================================================ //
// Headers
// ============================================================ //

// stop windows.h from decalaring min/max macros, thank you very much...
#include "../core/platform.hpp"
#if defined(LIGHTCTRL_PLATFORM_WINDOWS)
#define NOMINMAX
#endif

#define CHIF_NET_IMPLEMENTATION
#include "../thirdparty/chif/chif_net.h"
#include "../core/types.hpp"
#include "../core/buffer.hpp"
#include <string>
#include <stdexcept>

// post WINSOCK2 inclusion
#if defined(LIGHTCTRL_PLATFORM_WINDOWS)
#pragma comment(lib, "Ws2_32.lib")
#endif

// ====================================================================== //
// Exception Class
// ====================================================================== //

class socket_exception : public std::exception {

private:
  std::string m_error;

public:
  explicit socket_exception(std::string error) throw()
          : m_error(std::move(error)) {};

  virtual const char* what() const throw() { return m_error.c_str(); };
};

// ============================================================ //
// Class Declaration
// ============================================================ //

namespace lightctrl {

  class Tcp_socket {

  public:

    // ====================================================================== //
    // Variables and Constants
    // ====================================================================== //

  public:

    static constexpr s64 TCP_SOCKET_INVALID_SOCKET = CHIF_INVALID_SOCKET;

  private:

    chif_socket m_socket = TCP_SOCKET_INVALID_SOCKET;

    // ====================================================================== //
    // Lifetime Methods
    // ====================================================================== //

  public:

    /**
     * Constructor
     */
    explicit Tcp_socket() = default;

    // Delete copy constructor
    Tcp_socket(const Tcp_socket &other) = delete;

    // Move constructor
    Tcp_socket(Tcp_socket &&other) noexcept : m_socket(other.m_socket) {
      other.m_socket = CHIF_INVALID_SOCKET;
    };

    // Delete copy assignment
    Tcp_socket &operator=(const Tcp_socket &other) = delete;

    // move assignment
    Tcp_socket &operator=(Tcp_socket &&other) noexcept {
      m_socket = other.m_socket;
      return *this;
    };

    /**
     * Close the socket on destruction.
     */
    ~Tcp_socket() { close(); };

    // ====================================================================== //
    // Public Methods
    // ====================================================================== //

  public:

    /**
     * Open the socket. Will be called at construction.
     * Will throw on failure.
     */
    void open();

    /**
     * Close the socket. Will be called at destruction
     * Will throw on failure.
     */
    void close();

    /**
     * Read from the socket
     * Will throw on failure.
     * @return bytes read
     */
    ssize_t read(u8* buffer, size_t size);

    /**
     * Amount of read bytes will be stored in buffer.size()
     * Read from the socket
     * Will throw on failure.
     * @param buffer
     */
    void read(Buffer<u8> &buffer);

    /**
     * Write to the socket.
     * Will throw on failure.
     * @param buffer
     * @param size
     * @return bytes written
     */
    ssize_t write(const uint8_t *buffer, size_t size);

    /**
     * Write to the socket.
     * Will throw on failure.
     * @param buffer
     * @return bytes written
     */
    ssize_t write(const Buffer<u8> &buffer);

    /**
     * Attempt to establish a connection to the remote host.
     * Will throw on failure.
     */
    void connect(chif_net_address &address);
    void connect(const std::string &ip_address, u16 port);

    /**
     * Assign our socket an address.
     * Will throw on failure.
     */
    void bind(u16 port);

    /**
     * Set the socket state to LISTENING and make it ready to accept connections.
     * Will throw on failure.
     */
    void listen();

    /**
     * Create a new socket for any connection waiting to connect.
     * Note, this is a blocking call if there is no connection waiting & blocking
     * is set to true (default). Use can_accept() to see if there is a socket
     * waiting to be accepted.
     *
     * Will throw on failure.
     *
     * @return On success, a connected socket. On fail, the tcp_socket will have
     *         an INVALID_SOCKET socket.
     *         You can check the error with has_error() on the returned socket.
     */
    Tcp_socket accept();

    /**
     * Will throw on failure.
     * @retval true There is data waiting to be read.
     * @retval false There are no data to be read  and a call to read will block.
     */
    bool can_read();

    /**
     * Will throw on failure.
     * @retval true There is a connection waiting to be accepted.
     * @retval false There is no connection and a call to accept will block.
     */
    bool can_accept();

    /**
     * Will throw on failure.
     * @retval true There is an error in the socket.
     * @retval false There is no error in the socket.
     */
    bool has_error();

    /**
     * @return Return IP address and port. Example: "127.0.0.1:1337"
     */
    std::string get_address();

    /**
     * be careful!
     */
    void set_socket(chif_socket socket) { m_socket = socket; };

    /**
     * Do we have a valid socket? Note; to check for errors, use has_error
     * @return yes / no
     */
    bool is_valid() { return m_socket != INVALID_SOCKET; };

    void set_reuse_addr(bool reuse);

    static void win_init();

    static void win_shutdown() { chif_net_shutdown(); };

  };

}

#endif //LIGHTCTRL_BACKEND_TCP_SOCKET_HPP
