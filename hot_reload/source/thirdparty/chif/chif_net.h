// MIT License
//
// Copyright (c) 2017 Filip Björklund
// Copyright (c) 2017 Christoffer Gustafsson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef CHIF_NET_H
#define CHIF_NET_H

/**  \file chif_net.h
 * \date 2017-12-11
 * \authors Filip Björklund, Christoffer Gustafsson
 * \description
 * This is a single-header library inspired by the stb libraries by Sean barret.
 *
 * \usage
 * To use the library CHIF_NET_IMPLEMENTATION needs to be defined in one file,
 * ONLY one file, before this header is included.
 *
 *	After that all functions in the library may be used.
 *
 * \examples
 *
 *
 */

#if defined(__cplusplus)
extern "C" {
#endif

// ====================================================================== //
// Start of header
// ====================================================================== //

// Platform detection
#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW64__)
# define WIN32_LEAN_AND_MEAN // stop windows.h
#	define CHIF_WINDOWS
#	define CHIF_WINSOCK2
#	include <WinSock2.h>
#	include <WS2tcpip.h>
# include <winerror.h>
# include <ws2def.h>
#elif defined(__linux__) || defined (__APPLE__)
#	define CHIF_BERKLEY_SOCKET
#	include <sys/socket.h>
#	include <errno.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/ioctl.h>
#endif

// Inline
#if defined(CHIF_WINDOWS)
#	define CHIF_INLINE inline
#else
#	define CHIF_INLINE inline
#endif

#include <stdint.h>
#include <assert.h>

// ====================================================================== //
// Type-definitions
// ====================================================================== //

// Boolean type
typedef unsigned char chif_bool;

#if defined(CHIF_WINSOCK2)
// Socket type
typedef SOCKET chif_socket;
typedef int ssize_t;
#elif defined(CHIF_BERKLEY_SOCKET)
// Socket type
typedef int chif_socket;
typedef timeval TIMEVAL;
#endif

// ====================================================================== //
// Macro declarations
// ====================================================================== //

// True value
#define CHIF_TRUE ((chif_bool)1)
// False value
#define CHIF_FALSE ((chif_bool)0)

#if defined(CHIF_WINSOCK2)
// Invalid socket value
#	define CHIF_INVALID_SOCKET ((chif_socket)INVALID_SOCKET)
// Socket error value
#	define CHIF_SOCKET_ERROR ((int32_t)SOCKET_ERROR)
#elif defined(CHIF_BERKLEY_SOCKET)
// Invalid socket value
#	define CHIF_INVALID_SOCKET ((chif_socket)-1)
// Socket error value
#	define CHIF_SOCKET_ERROR ((int32_t)-1)
#endif

// default argument for listen
#define CHIF_DEFAULT_MAXIMUM_BACKLOG 128

// Minimum string length for when translating an iv4 socket struct address to string address representation.
#define CHIF_IPV4_STRING_LENGTH INET_ADDRSTRLEN
// Minimum string length for when translating an iv6 socket struct address to string address representation.
#define CHIF_IPV6_STRING_LENGTH INET6_ADDRSTRLEN
// Can hold both ipv4 and ipv6 addresses represented as a string.
#define CHIF_IPVX_STRING_LENGTH INET6_ADDRSTRLEN

// ====================================================================== //
// Data declarations
// ====================================================================== //

/** \enum chif_net_result_t
 * \author Filip Björklund
 * \date 11 december 2017 - 22:36
 * \brief Result enumeration.
 * \details Enumeration of results that are returned from some of the functions in
 * the chif_net library.
 */
typedef enum {
  /** Unknown result **/
          CHIF_RESULT_UNKNOWN = 0,
  /** Result of success **/
          CHIF_RESULT_SUCCESS,
  /** Library has not been initialized **/
          CHIF_RESULT_LIBRARY_NOT_INITIALIZED,
  /** Blocking call in progress **/
          CHIF_RESULT_BLOCKING,
  /** Maximum number of sockets reached **/
          CHIF_MAX_SOCKETS_REACHED,
  /** The object is not a socket **/
          CHIF_RESULT_NOT_A_SOCKET,
  /** The socket is set as non-blocking and the call would block. **/
          CHIF_RESULT_WOULD_BLOCK,
  /** Remote refused connetion. **/
          CHIF_RESULT_CONNECTION_REFUSED,
  /** Failed to use given address **/
          CHIF_RESULT_INVALID_ADDRESS,
  /** There is no file descriptor associated with the socket value. **/
          CHIF_RESULT_INVALID_FILE_DESCRIPTOR,
  /** Access denied. **/
          CHIF_RESULT_ACCESS_DENIED,
  /** Socket is already in use. **/
          CHIF_RESULT_SOCKET_ALREADY_IN_USE,
  /** There are no available ports. **/
          CHIF_RESULT_NO_FREE_PORT,
  /** Cannot take actions since another is in progress. **/
          CHIF_RESULT_IN_PROGRESS,
  /** Socket is already connected. **/
          CHIF_RESULT_ALREADY_CONNECTED,
  /** Action has timed out. **/
          CHIF_RESULT_TIMEDOUT,
  /** Connection has been aborted. **/
          CHIF_RESULT_CONNECTION_ABORTED,
  /** Socket is not listening for data. **/
          CHIF_RESULT_NOT_LISTENING_OR_NOT_CONNECTED,
  /** Cannot open any more sockets. **/
          CHIF_RESULT_NO_FREE_FILE_DESCRIPTORS,
  /** The system limit on the total number of open files has been reached. **/
          CHIF_RESULT_NO_FREE_FILES,
  /** Connection was reset by remote. **/
          CHIF_RESULT_SOCKET_RESET,
  /** Connection is closed **/
          CHIF_RESULT_CONNECTION_CLOSED,
  /** Not valid address family **/
          CHIF_RESULT_NOT_VALID_ADDRESS_FAMILY,
  /** There were not enugh memory/space available to performe the operation. **/
          CHIF_RESULT_NOT_ENOUGH_SPACE

} chif_net_result;

/** \enum chif_net_protocol_t
 * \author Filip Björklund
 * \date 11 december 2017 - 22:36
 * \brief Transport layer protocol enumeration.
 * \details
 * Enumeration of transport layer protocols that is used by sockets when sending
 * and receiving data over the network.
 */
typedef enum {
  /** TCP transport protocol **/
          CHIF_PROTOCOL_TCP,
  /** UDP transport protocol **/
          CHIF_PROTOCOL_UDP,
} chif_net_protocol;

/** \enum chif_address_family
 * \author Filip Björklund
 * \date 11 december 2017 - 22:51
 * \brief IP address family enumeration.
 * \details
 * Enumeration of IP address families. The two available types families are IPv4
 * (IP version 4) and IPv6 (IP version 6).
 */
typedef enum {
  /** IPv4 address family **/
  CHIF_ADDRESS_FAMILY_IPV4,
  /** IPv6 address family **/
  CHIF_ADDRESS_FAMILY_IPV6,
  /** Unix domain address family **/
  CHIF_ADDRESS_FAMILY_UNIX
} chif_address_family;

/**
 * Address structure used in chif_net
 */
typedef struct {
  struct sockaddr_storage addr;
} chif_net_address;

// ====================================================================== //
// Function declarations
// ====================================================================== //

/** Called to start up the chif network library.
 * \brief Startup call.
 * \return true for success, false for failure.
 */
chif_bool chif_net_startup();

/** Called to shut down the chif network library.
 * \brief Shutdown call.
 * \return Result of the operation.
 */
chif_net_result chif_net_shutdown();

/** Open a socket that uses the specified transport protocol for data
 * transmission.
 * \brief Open socket.
 * \param transport_protocol Transport protocol to use.
 * \param address_family IP address family.
 * \param socket The opened socket.
 * \return Result of the operation.
 */
chif_net_result chif_net_open_socket(chif_socket *socket, chif_net_protocol transport_protocol,
                                     chif_address_family address_family);

/** Closes a socket that was previously opened with the open socket function.
 * \brief Close socket.
 * \param socket The socket to close.
 */
chif_net_result chif_net_close_socket(chif_socket *socket);

/**
 * Connect to a IP address.
 * @param socket Open a socket before call with chif_net_open_socket
 * @param address Prepare the address with chif_net_create_address
 * @return Result of the operation.
 */
chif_net_result chif_net_connect(chif_socket socket, chif_net_address address);

/**
 * Bind a socket to the port
 * @param socket
 * @param port
 * @return
 */
chif_net_result chif_net_bind(chif_socket socket, uint16_t port, chif_address_family address_family);

/**
 * Start listening for connections on a socket.
 * @param socket
 * @param maximum_backlog Queue length for sockets waiting to be accepted.
 *                        Use CHIF_DEFAULT_MAXIMUM_BACKLOG for default.
 * @return
 */
chif_net_result chif_net_listen(chif_socket socket, uint32_t maximum_backlog);

/**
 *
 * @param server_socket
 * @param client_address
 * @return Client socket. On error the returned socket will be CHIF_INVALID_SOCKET
 */
chif_socket chif_net_accept(chif_socket server_socket, chif_net_address *client_address);

/**
 * Read data from the socket. Will block if blocking is set.
 * @param socket
 * @param buffer
 * @param size
 * @param readBytes
 * @return Result of the operation.
 */
chif_net_result chif_net_read(chif_socket socket, uint8_t *buffer, size_t size, ssize_t *readBytes);

/**
 * Write to a socket.
 * @param socket
 * @param buffer
 * @param size
 * @param sent_bytes
 * @return Result of the operation.
 */
chif_net_result chif_net_write(chif_socket socket, const uint8_t *buffer, size_t size, ssize_t *sent_bytes);

/**
 * Can we read without blocking? Is there anything to read?
 * See chif_net_get_bytes_available to get amount of bytes that can be read.
 * @param socket
 * @param socket_can_read
 * @return
 */
chif_net_result chif_net_can_read(chif_socket socket,
                                  chif_bool *socket_can_read);

/**
 * Can we write without blocking?
 * If the socket is in listening state it will check if we can call accept without blocking.
 * @param socket
 * @param socket_can_write
 * @return
 */
chif_net_result chif_net_can_write(chif_socket socket,
                                   chif_bool *socket_can_write);

/**
 * Check if the socket has any errors.
 * @param socket
 * @param socket_has_error
 * @return
 */
chif_net_result chif_net_has_error(chif_socket socket,
                                   chif_bool *socket_has_error);

/** Sets the blocking mode of a socket to either blocking or non-blocking
 * depending on the specified flag.
 * \brief Set blocking mode of socket.
 * \param socket Socket to set blocking mode on.
 * \param blocking True to make operations on the socket blocking. False to make
 * operations on the socket non-blocking.
 * \return Result of the operation.
 */
chif_net_result chif_net_set_socket_blocking(chif_socket socket,
                                             chif_bool blocking);

/**
 *
 * @param address Pointer to a address struct, example: chif_net_address server_address{};
 * @param ip_address String representation of ip address "ddd.ddd.ddd.ddd". Example, "127.0.0.1"
 * @param port
 * @param address_family Example, ipv4
 * @return
 */
chif_net_result chif_net_create_address(chif_net_address *address, const char *ip_address,
                                        uint16_t port, chif_address_family address_family);

/**
 * From a socket, get the IP address of the remote device as a string. Example "127.0.0.1" if ipv4.
 * @param socket
 * @param address
 * @param address_len Should be at least CHIF_IPV4_STRING_LENGTH for ipv4, and CHIF_IPV6_STRING_LEGNTH for ipv6.
 * @return
 */
chif_net_result chif_net_get_peer_name(chif_socket socket, char address[], size_t address_len);

/**
 * From a socket, get the port of the remote device.
 * @param socket
 * @param port
 * @return
 */
chif_net_result chif_net_get_peer_port(chif_socket socket, uint16_t *port);

/**
 * From a socket, get the address information about it.
 * @param socket
 * @param address
 * @return
 */
chif_net_result chif_net_get_peer_address(chif_socket socket, chif_net_address *address);

/**
 * Get number of bytes available for read on given socket.
 * @param socket
 * @param bytes_available Amount of bytes available to read.
 * @return
 */
chif_net_result chif_net_get_bytes_available(chif_socket socket, int &bytes_available);

/**
 *
 * @param socket
 * @param reuse To reuse or not, 0 for no, 1 for yes.
 * @return
 */
chif_net_result chif_net_set_reuse_addr(chif_socket socket, chif_bool reuse);

/**
 * Not possible on windows platform.
 * @param socket
 * @param reuseTo reuse or not, 0 for no, 1 for yes.
 * @return
 */
#if defined(CHIF_BERKLEY_SOCKET)
chif_net_result chif_net_set_reuse_port(chif_socket socket, chif_bool reuse);
#endif

// ====================================================================== //
// Implementation
// ====================================================================== //

#if defined(CHIF_NET_IMPLEMENTATION)

CHIF_INLINE chif_net_result _chif_get_spefic_result_type() {
#if defined(CHIF_WINSOCK2)
  const DWORD error = GetLastError();
  // TODO handle more errors on windows
  switch (error)
  {
  case WSANOTINITIALISED:
    return CHIF_RESULT_LIBRARY_NOT_INITIALIZED;
  case WSAEMFILE:
    return CHIF_MAX_SOCKETS_REACHED;
  case WSAENOTSOCK:
    return CHIF_RESULT_NOT_A_SOCKET;
  case WSAEAFNOSUPPORT:
    return CHIF_RESULT_INVALID_ADDRESS;
  case WSAEACCES:
    return CHIF_RESULT_ACCESS_DENIED;
  case WSAEADDRINUSE:
    return CHIF_RESULT_SOCKET_ALREADY_IN_USE;
  case WSAEADDRNOTAVAIL:
    return CHIF_RESULT_INVALID_ADDRESS;
  case WSAEINPROGRESS:
    return CHIF_RESULT_IN_PROGRESS;
  case WSAENOBUFS:
    return CHIF_RESULT_NOT_ENOUGH_SPACE;
  default:
    return CHIF_RESULT_UNKNOWN;
  }
#elif defined(CHIF_BERKLEY_SOCKET)

  switch (errno) {
    case ENOTSOCK:
      return CHIF_RESULT_NOT_A_SOCKET;

    case EBADF:
      return CHIF_RESULT_INVALID_FILE_DESCRIPTOR;

    case EALREADY:
      return CHIF_RESULT_WOULD_BLOCK;

    case ECONNREFUSED:
      return CHIF_RESULT_CONNECTION_REFUSED;

    case EACCES:
      return CHIF_RESULT_ACCESS_DENIED;

    case EADDRINUSE:
      return CHIF_RESULT_SOCKET_ALREADY_IN_USE;

    case EAGAIN:
      return CHIF_RESULT_NO_FREE_PORT;

    case EISCONN:
      return CHIF_RESULT_ALREADY_CONNECTED;

    case EINPROGRESS:
      return CHIF_RESULT_IN_PROGRESS;

    case ETIMEDOUT:
      return CHIF_RESULT_TIMEDOUT;

    case ECONNABORTED:
      return CHIF_RESULT_CONNECTION_ABORTED;

    case EINVAL:
      return CHIF_RESULT_NOT_LISTENING_OR_NOT_CONNECTED;

    case EMFILE:
      return CHIF_RESULT_NO_FREE_FILE_DESCRIPTORS;

    case ENFILE:
      return CHIF_RESULT_NO_FREE_FILES;

    case ECONNRESET:
      return CHIF_RESULT_SOCKET_RESET;

    case ENOTCONN:
      return CHIF_RESULT_CONNECTION_CLOSED;

    case EAFNOSUPPORT:
      return CHIF_RESULT_NOT_VALID_ADDRESS_FAMILY;

    case ENOSPC:
      return CHIF_RESULT_NOT_ENOUGH_SPACE;

    default:
      return CHIF_RESULT_UNKNOWN;
  }
#else
  return CHIF_RESULT_UNKNOWN;
#endif
}

CHIF_INLINE chif_bool chif_net_startup(void) {
#if defined(CHIF_WINSOCK2)
  WSADATA winsock_data;
  const int32_t result = WSAStartup(WINSOCK_VERSION, &winsock_data);
  if (result != NO_ERROR)
  {
    return CHIF_FALSE;
  }
#endif
  return CHIF_TRUE;
}

CHIF_INLINE chif_net_result chif_net_shutdown() {
#if defined(CHIF_WINSOCK2)
  const int32_t result = WSACleanup();
  if (result == WSAEINPROGRESS) { return CHIF_RESULT_BLOCKING; }
  else if (result != 0) { return CHIF_RESULT_UNKNOWN; }
#endif
  return CHIF_RESULT_SUCCESS;
}

CHIF_INLINE chif_net_result chif_net_open_socket(chif_socket *socket_out, chif_net_protocol transport_protocol,
                                                 chif_address_family address_family) {
  // Get the transport protocol type
  int32_t _transport_protocol = SOCK_STREAM;
  if (transport_protocol == CHIF_PROTOCOL_UDP) { _transport_protocol = SOCK_DGRAM; }

  // Get the IP address family
  int32_t _address_family = AF_INET;
  if (address_family == CHIF_ADDRESS_FAMILY_IPV6) { _address_family = AF_INET6; }

  // Open the socket
  constexpr unsigned long ANY_IP_PROTOCOL = 0;
  const chif_socket result_socket = socket(_address_family, _transport_protocol, ANY_IP_PROTOCOL);

  if (result_socket == CHIF_INVALID_SOCKET)
    return _chif_get_spefic_result_type();

  *socket_out = result_socket;

  return CHIF_RESULT_SUCCESS;
}

CHIF_INLINE chif_net_result chif_net_close_socket(chif_socket *socket) {
  if (*socket != CHIF_INVALID_SOCKET) {
    // Close the socket
#if defined(CHIF_WINSOCK2)
    const int result = closesocket(*socket);
#elif defined(CHIF_BERKLEY_SOCKET)
    int result = close(*socket);
#endif

    // Set socket to invalid to prevent usage of closed socket.
    *socket = CHIF_INVALID_SOCKET;

    if (result == CHIF_SOCKET_ERROR)
      return _chif_get_spefic_result_type();
  }

  return CHIF_RESULT_SUCCESS;
}

CHIF_INLINE chif_net_result chif_net_connect(chif_socket socket, chif_net_address address) {
  const int result = connect(socket, (struct sockaddr *) &address.addr, sizeof(address.addr));

  if (result == CHIF_SOCKET_ERROR)
    return _chif_get_spefic_result_type();

  return CHIF_RESULT_SUCCESS;
}

CHIF_INLINE chif_net_result chif_net_bind(chif_socket socket, uint16_t port, chif_address_family address_family) {
  chif_net_address address{};
  //TODO address construction without string
  int result = chif_net_create_address(&address, "0.0.0.0", port, address_family);

  if (result == CHIF_SOCKET_ERROR)
    return _chif_get_spefic_result_type();

  result = bind(socket, (struct sockaddr*) &address, sizeof(address));

  if (result == CHIF_SOCKET_ERROR)
    return _chif_get_spefic_result_type();

  return CHIF_RESULT_SUCCESS;
}

CHIF_INLINE chif_net_result chif_net_listen(chif_socket socket, uint32_t maximum_backlog) {
  const int result = listen(socket, maximum_backlog);

  if (result == CHIF_SOCKET_ERROR)
    return _chif_get_spefic_result_type();

  return CHIF_RESULT_SUCCESS;
}

CHIF_INLINE chif_socket chif_net_accept(chif_socket server_socket, chif_net_address *client_address) {
  //TODO set client_address.sin_family
  chif_socket client_socket;
  socklen_t client_addrlen = sizeof(struct sockaddr_in);

  client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_addrlen);

  //TODO handle errors differently?
  if (client_socket == CHIF_INVALID_SOCKET) {
    return _chif_get_spefic_result_type();
  }

  return client_socket;
}

CHIF_INLINE chif_net_result chif_net_read(chif_socket socket, uint8_t *buffer, size_t size, ssize_t *read_bytes) {
  if (socket == CHIF_INVALID_SOCKET)
    return CHIF_RESULT_NOT_A_SOCKET;

  int flag = 0;
#if defined(CHIF_BERKLEY_SOCKET)
  // Prevent SIGPIPE signal and handle the error in application code
  flag = MSG_NOSIGNAL;
#endif

  const ssize_t result = recv(socket, (char*)buffer, (int)size, flag);

  if (result == CHIF_SOCKET_ERROR)
    return _chif_get_spefic_result_type();
  else if (!result)
    return chif_net_result::CHIF_RESULT_CONNECTION_CLOSED;

  *read_bytes = result;

  return CHIF_RESULT_SUCCESS;
}

CHIF_INLINE chif_net_result
chif_net_write(chif_socket socket, const uint8_t *buffer, size_t size, ssize_t *sent_bytes) {
  if (socket == CHIF_INVALID_SOCKET)
    return CHIF_RESULT_NOT_A_SOCKET;

  int flag = 0;
#if defined(CHIF_BERKLEY_SOCKET)
  // Prevent SIGPIPE signal and handle the error in application code
  flag = MSG_NOSIGNAL;
#endif

  const ssize_t result = send((int)socket, (char*)buffer, (int)size, flag);

  if (result == CHIF_SOCKET_ERROR)
    return _chif_get_spefic_result_type();

  *sent_bytes = result;

  return CHIF_RESULT_SUCCESS;
}

CHIF_INLINE chif_net_result chif_net_can_read(chif_socket socket,
                                              chif_bool *socket_can_read) {
  fd_set check_socket;

  FD_ZERO(&check_socket);
  FD_SET(socket, &check_socket);

  TIMEVAL timeout = {};

  int result = select((int)socket + 1, &check_socket, nullptr, nullptr, &timeout);

  if (result == CHIF_SOCKET_ERROR)
    return _chif_get_spefic_result_type();

  *socket_can_read = FD_ISSET(socket, &check_socket);

  return CHIF_RESULT_SUCCESS;
}

CHIF_INLINE chif_net_result chif_net_can_write(chif_socket socket,
                                               chif_bool *socket_can_write) {
  fd_set check_socket;

  FD_ZERO(&check_socket);
  FD_SET(socket, &check_socket);

  TIMEVAL timeout = {};

  int result = select((int)socket + 1, nullptr, &check_socket, nullptr, &timeout);

  if (result == CHIF_SOCKET_ERROR)
    return _chif_get_spefic_result_type();

  *socket_can_write = FD_ISSET(socket, &check_socket);

  return CHIF_RESULT_SUCCESS;
}

CHIF_INLINE chif_net_result chif_net_has_error(chif_socket socket,
                                               chif_bool *socket_has_error) {
  fd_set check_socket;

  FD_ZERO(&check_socket);
  FD_SET(socket, &check_socket);

  TIMEVAL timeout = {};

  int result = select((int)socket + 1, nullptr, nullptr, &check_socket, &timeout);

  if (result == CHIF_SOCKET_ERROR)
    return _chif_get_spefic_result_type();

  *socket_has_error = FD_ISSET(socket, &check_socket);

  return CHIF_RESULT_SUCCESS;
}

CHIF_INLINE chif_net_result chif_net_set_socket_blocking(chif_socket socket, chif_bool blocking) {
#if defined(CHIF_WINSOCK2)
  ULONG blocking_mode = !blocking;
  int32_t result = ioctlsocket(socket, FIONBIO, &blocking_mode);
  if (result == CHIF_SOCKET_ERROR)
  {
    return _chif_get_spefic_result_type();
  }
#elif defined(CHIF_BERKLEY_SOCKET)
  int32_t flags = fcntl(socket, F_GETFL, 0);
  blocking ? flags &= ~O_NONBLOCK : flags |= O_NONBLOCK;
  fcntl(socket, F_SETFL, flags);
#endif

  return CHIF_RESULT_SUCCESS;
}

CHIF_INLINE chif_net_result chif_net_create_address(chif_net_address *address, const char *ip_address,
                                                    uint16_t port, chif_address_family address_family) {
  // TODO support UNIX domain sockets
  int result;

  if (address_family == chif_address_family::CHIF_ADDRESS_FAMILY_IPV4) {
    (*address).addr.ss_family = AF_INET;
    struct sockaddr_in *addr_in = (struct sockaddr_in *) (&address->addr);
    result = inet_pton((*address).addr.ss_family, ip_address, &addr_in->sin_addr);
    (*addr_in).sin_port = htons(port);
  } else if (address_family == CHIF_ADDRESS_FAMILY_IPV6) {
    (*address).addr.ss_family = AF_INET6;
    struct sockaddr_in6 *addr_in6 = (struct sockaddr_in6 *) (&address->addr);
    result = inet_pton((*address).addr.ss_family, ip_address, &addr_in6->sin6_addr);
    (*addr_in6).sin6_port = htons(port);
  } else {
    return CHIF_RESULT_NOT_VALID_ADDRESS_FAMILY;
  }

  if (result != 1) {
    if (result == -1) {
      return _chif_get_spefic_result_type();
    } else if (result == 0) {
      return CHIF_RESULT_INVALID_ADDRESS;
    }
    return CHIF_RESULT_UNKNOWN;
  }

  return CHIF_RESULT_SUCCESS;
}

CHIF_INLINE chif_net_result chif_net_get_peer_name(chif_socket socket, char address[], size_t address_len) {
  chif_net_address net_address{};
  chif_net_result result = chif_net_get_peer_address(socket, &net_address);
  if (result != CHIF_RESULT_SUCCESS)
    return result;

  const char *ntop_result = nullptr;
  if (net_address.addr.ss_family == AF_INET) {
    struct sockaddr_in *addr_ivp4 = (struct sockaddr_in *) &net_address.addr;
    ntop_result = inet_ntop(net_address.addr.ss_family, &addr_ivp4->sin_addr, address, address_len);
  } else if (net_address.addr.ss_family == AF_INET6) {
    struct sockaddr_in6 *addr_ivp6 = (struct sockaddr_in6 *) &net_address.addr;
    ntop_result = inet_ntop(net_address.addr.ss_family, &addr_ivp6->sin6_addr, address, address_len);
  }

  if (ntop_result == NULL) {
    return _chif_get_spefic_result_type();
  }

  return CHIF_RESULT_SUCCESS;
}

CHIF_INLINE chif_net_result chif_net_get_peer_port(chif_socket socket, uint16_t *port) {
  chif_net_address net_address{};
  chif_net_result result = chif_net_get_peer_address(socket, &net_address);
  if (result != CHIF_RESULT_SUCCESS)
    return result;

  if (net_address.addr.ss_family == AF_INET) {
    struct sockaddr_in *addr_ivp4 = (struct sockaddr_in *) &net_address.addr;
    *port = ntohs(addr_ivp4->sin_port);
  } else if (net_address.addr.ss_family == AF_INET6) {
    struct sockaddr_in6 *addr_ivp6 = (struct sockaddr_in6 *) &net_address.addr;
    *port = ntohs(addr_ivp6->sin6_port);
  }

  return CHIF_RESULT_SUCCESS;
}

CHIF_INLINE chif_net_result chif_net_get_peer_address(chif_socket socket, chif_net_address *address) {
  struct sockaddr *addr = (struct sockaddr *) &address->addr;
  socklen_t addr_len = sizeof(sockaddr_storage);
  int result = getpeername(socket, addr, &addr_len);

  if (addr_len > sizeof(sockaddr_storage)) {
    // address was truncated because lack of storage in addr
    return CHIF_RESULT_UNKNOWN;
  }

  // TODO use CHIF_SOCKET_ERROR or -1?
  if (result == CHIF_SOCKET_ERROR) {
    return _chif_get_spefic_result_type();
  }

  return CHIF_RESULT_SUCCESS;
}

CHIF_INLINE chif_net_result chif_net_get_bytes_available(chif_socket socket, int &bytes_available) {
  int result = CHIF_SOCKET_ERROR; //TODO when windows implementation is done, only declare.
#if defined(CHIF_WINDOWS)
  //TODO implement on windows
  assert(false && "Not implemented");
#elif defined(CHIF_BERKLEY_SOCKET)
  result = ioctl(socket, FIONREAD, &bytes_available);
#endif

  if (result == CHIF_SOCKET_ERROR) {
    return _chif_get_spefic_result_type();
  }

  return chif_net_result::CHIF_RESULT_SUCCESS;
}

CHIF_INLINE chif_net_result chif_net_set_reuse_addr(chif_socket socket, chif_bool reuse) {
  const ssize_t result = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(int));

  if (result < 0) {
    return _chif_get_spefic_result_type();
  }

  return CHIF_RESULT_SUCCESS;
}

#if defined(CHIF_BERKLEY_SOCKET)
CHIF_INLINE chif_net_result chif_net_set_reuse_port(chif_socket socket, chif_bool reuse) {
  const ssize_t result = setsockopt(socket, SOL_SOCKET, SO_REUSEPORT, (char*)&reuse, sizeof(int));

  if (result < 0) {
    return _chif_get_spefic_result_type();
  }

  return CHIF_RESULT_SUCCESS;
}
#endif

#endif

// ====================================================================== //
// End of implementation
// ====================================================================== //

#if defined(__cplusplus)
}
#endif

#endif // CHIF_NET_H
