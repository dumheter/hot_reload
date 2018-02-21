#ifndef LIGHTCTRL_BACKEND_TCP_PACKET_HPP
#define LIGHTCTRL_BACKEND_TCP_PACKET_HPP

// ====================================================================== //
// Headers
// ====================================================================== //

#include "../core/types.hpp"
#include "../core/buffer.hpp"

// ====================================================================== //
// Class Declaration
// ====================================================================== //

/**
 * Tcp packet
 *
 *  Overall structure:
 *   ______________________
 *  | header | payload ... |
 *  |________|_____________|
 *
 *  Header:
 *   _________________________
 *  |signature | payload size |
 *  |__________|______________|
 *
 *  The header contains a type, and a paylaod size.
 *  The type tells us how the packet should be handled.
 *  The payload size tells us how long the payload is. This is useful
 *  for when receiving a packet that is longer than the maximum ethernet/wifi
 *  packet size.
 *
 *  Payload:
 *  This simply contains the applications payload.
 */
class Tcp_packet {

  // ====================================================================== //
  // Data Types Declaration
  // ====================================================================== //

public:

  enum class Packet_signature : u8 {
    INVALID = 0,
    HANDSHAKE,
    DISCONNECT,
    PING,
    PONG,
    REQUEST,
    RESPONSE,

    // used to validate packet signatures, lower values are valid.
    VALID_PACKET_SIGNATURE_HELPER
  };

private:

  /** Must have same ordering and same members as Packet_signature **/
  static const char* Packet_signature_string_name[];

  // ====================================================================== //
  // Variables and Constants
  // ====================================================================== //

private:

  /** Packet header **/
  struct Header{
    u8 signature = static_cast<u8>(Packet_signature::INVALID);
    u16 payload_size = 0;
  };

  /** Holds everything, both the header and the payload. **/
  Buffer<u8> m_packet;

  static constexpr u64 PAYLOAD_OFFSET = sizeof(Header);

  // ====================================================================== //
  // Lifetime Methods
  // ====================================================================== //

public:

  /**
   * Construct Tcp payload with empty buffer.
   * Can be used with parse_packet methods.
   */
  Tcp_packet();

  /**
   *
   * @param packet_signature
   * @param capacity Reserve memory for buffer
   */
  explicit Tcp_packet(Packet_signature packet_signature, u64 capacity);

  /**
   * Construct from signature and payload
   * @param packet_signature
   * @param payload Will copy the payload
   */
  explicit Tcp_packet(Packet_signature packet_signature,
                      const Buffer<u8>& payload);

  /**
   * Construct a Tcp packet with a signature and payload
   * @param packet_signature
   * @param payload Your application data
   * @param size
   */
  Tcp_packet(Packet_signature packet_signature, const u8* payload,
                      u64 size);

  // ====================================================================== //
  // Getters and Setters
  // ====================================================================== //

public:

  /**
   * Will return a sub-buffer of the packet containing only the payload.
   * Do not hold on to the sub-buffer as it might be invalid past the scope.
   * @return Buffer with the payload.
   */
  Buffer<u8>&& get_payload();

  /**
   * @return Copy of the payload represented as a string
   */
  std::string get_payload_as_string();

  /** Set payload by copy **/
  void set_payload(const Buffer<u8>& payload);

  /**
   * Set payload by copy
   * @param payload
   * @param size
   */
  void set_payload(const u8* payload, u64 size);

  /** Get packet type, read from header **/
  Packet_signature get_signature() const;

  const char* get_signature_as_string() const;

  /**
   * Set packet type, stored in header
   * @param packet_signature If not valid type, the type invalid_type will be set
   *                    and recipient of the packet are likely to discard the packet.
   */
  void set_signature(Packet_signature packet_signature);

  /** Return total size of packet, header + payload **/
  u64 get_packet_size() const;

  /** Return size of payload. Use get_total_size to see how much memory is used. **/
  u16 get_payload_size() const;

  /**
   * Convert a raw u8 data stream into a tcp_packet.
   * @pre packet must be heap allocated.
   * @param packet Takes ownership of this data
   * @param size size of packet
   */
  void parse_packet(u8* packet, u64 size);

  Buffer<u8>& get_buffer();

  Header get_header();

  void set_header(const Header& new_header);

  // ====================================================================== //
  // Private Methods
  // ====================================================================== //

private:

  /**
   * Copy payload into new buffer
   * @pre payload size must be less than maximum header size value. (2^16)
   * @param payload
   * @param size
   */
  void write_payload_and_update_size(const Buffer<u8>& payload);
  void write_payload_and_update_size(const u8* payload, u64 size);

  /**
   * Get maximum possible size of the packet (header + paylaod).
   *
   * @bugs: It might be possible to cause overflow is header is maximum u64 size.
   **/
  u64 get_total_max_size();

  /**
   * @pre payload_size must be smaller than get_total_max_size() - HEADER_SIZE.
   *      and larger than HEADER_SIZE
   * @param payload_size
   */
  void set_payload_size(u16 payload_size);

  /**
   * Check if the packet signature is among the valid packet signatures.
   * Valid packet signatures are defined in Packet_signature enum class.
   * @param packet_signature The signature to check.
   * @return Valid or not.
   */
  bool valid_signature(Packet_signature packet_signature);

  bool valid_header();

  /**
   * Write 0's to header section.
   */
  void clear_header();

  // ============================================================ //

};

#endif //LIGHTCTRL_BACKEND_TCP_PACKET_HPP
