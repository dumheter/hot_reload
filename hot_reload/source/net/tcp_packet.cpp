#include <cstring>
#include <stdexcept>
#include "tcp_packet.hpp"

// ====================================================================== //
// Class Implementation
// ====================================================================== //

const char* Tcp_packet::Packet_signature_string_name[] = {
        "invalid",
        "handshake",
        "disconnect",
        "ping",
        "pong",
        "request",
        "response"
};

Tcp_packet::Tcp_packet() : m_packet() {}

// ============================================================ //

Tcp_packet::Tcp_packet(Packet_signature packet_signature, u64 capacity)
        : m_packet(capacity) {
  clear_header();
  set_signature(packet_signature);
}

// ============================================================ //

Tcp_packet::Tcp_packet(Packet_signature packet_signature, const Buffer<u8>& payload)
        : m_packet(payload.size() + sizeof(Header)) {
  clear_header();
  write_payload_and_update_size(payload);
  set_signature(packet_signature);
}

// ============================================================ //

Tcp_packet::Tcp_packet(Tcp_packet::Packet_signature packet_signature,
                       const u8 *payload, u64 size)
: m_packet(size + sizeof(Header)) {
  clear_header();
  write_payload_and_update_size(payload, size);
  set_signature(packet_signature);
}

// ============================================================ //

Buffer<u8>&& Tcp_packet::get_payload() {
  if (!m_packet.size()) throw std::runtime_error("cannot read payload from an empty packet");
  return m_packet.make_sub_buffer(sizeof(Header));
}

// ============================================================ //

std::string Tcp_packet::get_payload_as_string() {
  if (!m_packet.size()) throw std::runtime_error("cannot read payload from an empty packet");
  return this->get_payload().as_string();
}

// ============================================================ //

void Tcp_packet::set_payload(const Buffer<u8>& payload) {
  write_payload_and_update_size(payload);
}

// ============================================================ //

void Tcp_packet::set_payload(const u8* payload, u64 size) {
  write_payload_and_update_size(payload, size);
}

// ============================================================ //

Tcp_packet::Packet_signature Tcp_packet::get_signature() const {
  if (!m_packet.size()) throw std::runtime_error("cannot read signature from an empty packet");
  const Header* header = reinterpret_cast<Header*>(m_packet.raw());
  return static_cast<Packet_signature>(header->signature);
}

// ============================================================ //

const char* Tcp_packet::get_signature_as_string() const {
  if (!m_packet.size()) throw std::runtime_error("cannot read signature from an empty packet");
  const Header* header = reinterpret_cast<Header*>(m_packet.raw());
  return Packet_signature_string_name[header->signature];
}

// ============================================================ //

void Tcp_packet::set_signature(Packet_signature packet_signature) {
  if (!m_packet.capacity()) {
    m_packet.resize(sizeof(Header), true);
  }

  if (!valid_signature(packet_signature)) packet_signature = Packet_signature::INVALID;
  Header* header = reinterpret_cast<Header*>(m_packet.raw());
  header->signature = static_cast<u8>(packet_signature);
}

// ============================================================ //

u64 Tcp_packet::get_packet_size() const {
  return m_packet.size();
}

// ============================================================ //

u16 Tcp_packet::get_payload_size() const {
  if (!m_packet.size()) throw std::runtime_error("cannot read size from an empty packet");
  const Header* header = reinterpret_cast<Header*>(m_packet.raw());
  return header->payload_size;
}

// ============================================================ //

void Tcp_packet::parse_packet(u8 *packet, u64 size) {
  m_packet.move_set(packet, size, size);
}

Buffer<u8> &Tcp_packet::get_buffer() {
  return m_packet;
}

// ============================================================ //

Tcp_packet::Header Tcp_packet::get_header() {
  Header* header = reinterpret_cast<Header*>(m_packet.raw());
  return *header;
}

void Tcp_packet::set_header(const Tcp_packet::Header& new_header) {
  Header* header = reinterpret_cast<Header*>(m_packet.raw());
  header->signature = new_header.signature;
  header->payload_size = new_header.payload_size;
}

// ====================================================================== //
// Private Methods
// ====================================================================== //

void Tcp_packet::write_payload_and_update_size(const Buffer<u8> &payload) {
  if (payload.size() + PAYLOAD_OFFSET > m_packet.capacity()) {
    if (payload.size() + PAYLOAD_OFFSET > get_total_max_size()) {
      throw std::runtime_error("payload too large for the tcp packet to carry."
      " You need to increase the size parameter in the header of the tcp_packet.");
    }
    m_packet.resize(payload.size() + PAYLOAD_OFFSET, false);
  }

  m_packet.copy_set(payload.raw(), payload.size() + PAYLOAD_OFFSET,
                    payload.size() + PAYLOAD_OFFSET, PAYLOAD_OFFSET);
  set_payload_size(static_cast<u16>(payload.size())); // type mismatch note: will fit, else throws runtime_error
}

// ============================================================ //

void Tcp_packet::write_payload_and_update_size(const u8* payload, u64 size) {
  if (size + PAYLOAD_OFFSET > m_packet.size() &&
          m_packet.size() < get_total_max_size()) {
    m_packet.resize(size + PAYLOAD_OFFSET, false);
  }

  m_packet.copy_set(payload, size + PAYLOAD_OFFSET,
                    size + PAYLOAD_OFFSET, PAYLOAD_OFFSET);
  set_payload_size(static_cast<u16>(size));
}

// ============================================================ //

u64 Tcp_packet::get_total_max_size() {
  return (1UL << sizeof(u16)*8) - 1 + PAYLOAD_OFFSET;
}

// ============================================================ //

void Tcp_packet::set_payload_size(u16 payload_size) {
  if (m_packet.size() < PAYLOAD_OFFSET) return;
  Header* header = reinterpret_cast<Header*>(m_packet.raw());

  header->payload_size = payload_size;
}

// ============================================================ //

bool Tcp_packet::valid_signature(Tcp_packet::Packet_signature packet_signature) {
  return (packet_signature < Packet_signature::VALID_PACKET_SIGNATURE_HELPER);
}

// ============================================================ //

bool Tcp_packet::valid_header() {
  const Header* header = reinterpret_cast<Header*>(m_packet.raw());

  if (!valid_signature(static_cast<Packet_signature>(header->signature))) {
    return false;
  }

  if (header->payload_size > m_packet.size()) {
    return false;
  }

  return true;
}

void Tcp_packet::clear_header() {
  memset(m_packet.raw(), 0, sizeof(Header));
}
