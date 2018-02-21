#ifndef LIGHTCTRL_BACKEND_BUFFER_HPP
#define LIGHTCTRL_BACKEND_BUFFER_HPP

// ====================================================================== //
// Headers
// ====================================================================== //

#include "../core/types.hpp"
#include <string>
#include <cstring>
#include <stdexcept>
#include <algorithm>

// ====================================================================== //
// Class Declaration
// ====================================================================== //

template <typename Buffer_type>
class Buffer {

  // ====================================================================== //
  // Variables and Constants
  // ====================================================================== //

public:

  using flag = u8;
  static constexpr flag FLAG_NO_FLAGS = 0;
  static constexpr flag FLAG_CLEAR = 1;

private:

  /** The underlying buffer **/
  Buffer_type* m_buffer;

  /** The maximum amount of bytes stored in the buffer **/
  u64 m_capacity;

  /** The number of used bytes, cannot exceed m_capacity **/
  u64 m_size;

  /** If we should delete m_buffer on destruction, used by sub buffers **/
  bool m_own_buffer = true;

  // ====================================================================== //
  // Lifetime methods
  // ====================================================================== //

public:

  /** Normal construction **/
  explicit Buffer(u64 capacity, flag flag = FLAG_NO_FLAGS);

  /** Construct from string by copy **/
  explicit Buffer(const std::string& string);

  /**
   * Construct without allocation.
   * @param buffer Takes ownership of this buffer.
   * @param capacity Size of new buffer
   * @param size Used bytes
   * @param delete_on_destruct Should we delete m_buffer on destruction?
   *                           If constructing as a sub buffer, set to false
   */
  Buffer(Buffer_type* buffer, u64 capacity, u64 size, bool delete_on_destruct = true);

  /** Construct empty buffer **/
  Buffer();

  /** Copy constructor **/
  Buffer(const Buffer& other);

  /** Move constructor **/
  Buffer(Buffer&& other) noexcept;

  /** Delete buffer **/
  ~Buffer();

  /** Copy assignment **/
  Buffer& operator=(const Buffer& other);

  /** move assignment **/
  Buffer& operator=(Buffer&& other) noexcept;

  /**
   * Create a sub buffer. Will not delete buffer on destruction.
   * Destructing parent before you are done with sub buffer may cause SEGFAULT.
   **/
  Buffer&& make_sub_buffer(u64 offset);


// ====================================================================== //
// Misc methods
// ====================================================================== //

public:

  /** Overwrite used buffer with 0's (m_size) **/
  void clear();

  /** Overwrite whole buffer with 0's (m_capacity) **/
  void clear_total();

  /**
   * Delete old buffer and make a new one.
   * Cannot be done from sub buffer.
   * @param capacity New capacity
   * @param copy_old Copy the old buffer to the new resized buffer. Will only
   * copy up to m_size, will ignore the difference betweeen m_size and m_capacity.
   */
  void resize(u64 capacity, bool copy_old);

  // ====================================================================== //
  // Getters and Setters
  // ====================================================================== //

public:

  /** Return raw pointer **/
  Buffer_type* raw() const { return m_buffer; };
  
  /** Return size of used buffer **/
  u64 size() const { return m_size; };

  /**
   * Set m_size, if larger than m_capacity, call resize
   *
   * @throw If trying to set a size larger than capacity on a sub buffer an
   * exception will be thrown.
   **/
  void set_size(u64 size);

  /** Return size of buffer **/
  u64 capacity() const { return m_capacity; };

  /**
   * Delete current buffer and make new one with a copy of data
   * @pre Size of data must be size - offset.
   * @param data Set to copy to new buffer.
   * @param size Total size of new buffer.
   * @param offset Put the data at an offset.
   * @return
   */
  void copy_set(const Buffer_type* data, u64 capacity, u64 size, u64 offset = 0);
  
  /** Delete current buffer and make our buffer point to data
   * @pre Data must be heap allocated since it will call free.
   * @param data
   * @param capacity
   * @param size
   **/
  void move_set(Buffer_type* data, u64 capacity, u64 size);
  
  /** Get copy as string **/
  std::string as_string() const;

  /** Set buffer from string **/
  void from_string(const std::string& string);
  
  /** Array index operator overload **/
  Buffer_type& operator[](u64 index);

};

// ====================================================================== //
// Class Template Implementation
// ====================================================================== //

template <typename Buffer_type>
Buffer<Buffer_type>::Buffer(u64 capacity, typename::Buffer<Buffer_type>::flag flag) :
        m_buffer(new Buffer_type[static_cast<unsigned int>(capacity)]), m_capacity(capacity), m_size(0) {
  if (flag == FLAG_CLEAR) clear_total();
}

// ============================================================ //

template <typename Buffer_type>
Buffer<Buffer_type>::Buffer(const std::string& string) :
        m_buffer(new Buffer_type[string.size()]), m_capacity(string.size()),
        m_size(string.size()) {
  memcpy(m_buffer, string.c_str(), m_size);
}

// ============================================================ //

template <typename Buffer_type>
Buffer<Buffer_type>::Buffer(Buffer_type* buffer, u64 capacity, u64 size, bool delete_on_destruct) :
        m_buffer(buffer), m_capacity(capacity), m_size(size),
        m_own_buffer(delete_on_destruct) {}

// ============================================================ //

template <typename Buffer_type>
Buffer<Buffer_type>::Buffer() : m_buffer(nullptr), m_capacity(0), m_size(0) {}

// ============================================================ //

template <typename Buffer_type>
Buffer<Buffer_type>::Buffer(const Buffer<Buffer_type> &other) {
  m_buffer = new Buffer_type[other.m_capacity];
  m_capacity = other.m_capacity;
  m_size = other.m_size;
  memcpy(m_buffer, other.m_buffer, m_size);
}

// ============================================================ //

template <typename Buffer_type>
Buffer<Buffer_type>::Buffer(Buffer<Buffer_type> &&other) noexcept {
  m_buffer = other.m_buffer;
  m_capacity = other.m_capacity;
  m_size = other.m_size;
  m_own_buffer = other.m_own_buffer;
  other.m_buffer = nullptr;
  other.m_capacity = 0;
  other.m_size = 0;
}

// ============================================================ //

template <typename Buffer_type>
Buffer<Buffer_type>::~Buffer() {
  if (m_own_buffer) {
    delete[] m_buffer;
  }
}

// ============================================================ //

template <typename Buffer_type>
Buffer<Buffer_type> &Buffer<Buffer_type>::operator=(const Buffer<Buffer_type> &other) {
  if (this == &other) return *this;

  if (m_capacity != other.m_capacity) {
    delete[] m_buffer;
    m_buffer = new Buffer_type[other.m_capacity];
    m_capacity = other.m_capacity;
  }

  m_size = other.m_size;
  memcpy(m_buffer, other.m_buffer, m_size);

  return *this;
}

// ============================================================ //

template <typename Buffer_type>
Buffer<Buffer_type> &Buffer<Buffer_type>::operator=(Buffer<Buffer_type> &&other) noexcept {
  delete[] m_buffer;
  m_buffer = other.m_buffer;
  m_capacity = other.m_capacity;
  m_size = other.m_size;
  m_own_buffer = other.m_own_buffer;
  other.m_buffer = nullptr;
  other.m_capacity = 0;
  other.m_size = 0;
  return *this;
}

// ============================================================ //

template <typename Buffer_type>
Buffer<Buffer_type>&& Buffer<Buffer_type>::make_sub_buffer(u64 offset) {
  Buffer sub_buffer = Buffer(m_buffer + offset, m_capacity - offset,
                             m_size < offset ? 0 : m_size - offset, false);
  return std::move(sub_buffer);
}


// ============================================================ //

template <typename Buffer_type>
void Buffer<Buffer_type>::clear() {
  memset(m_buffer, 0, m_size);
}

// ============================================================ //

template <typename Buffer_type>
void Buffer<Buffer_type>::clear_total() {
  memset(m_buffer, 0, static_cast<size_t>(m_capacity));
}

// ============================================================ //

template <typename Buffer_type>
void Buffer<Buffer_type>::set_size(u64 size) {
  if (size > m_capacity) {
    if (m_own_buffer) {
      this->resize(size, true);
    }
    else {
      throw std::runtime_error(
              "Trying to set size larger than capacity, and does not own "
                      "buffer. Likely tring to set size on a sub buffer."
      );
    }
  }

  m_size = size;
}

// ============================================================ //

template <typename Buffer_type>
void Buffer<Buffer_type>::resize(u64 capacity, bool copy_old) {
  if (!m_own_buffer) return;

  Buffer_type* new_buffer = new Buffer_type[static_cast<unsigned int>(capacity)];

  if (m_buffer && copy_old) {
    memcpy(new_buffer, m_buffer, static_cast<size_t>(std::min(capacity, m_size)));
  }
  delete[] m_buffer;

  m_buffer = new_buffer;
  m_capacity = capacity;

  if (m_size > m_capacity) {
    m_size = m_capacity;
  }
}

// ============================================================ //

template <typename Buffer_type>
void Buffer<Buffer_type>::copy_set(const Buffer_type *data, u64 capacity, u64 size, u64 offset) {
  delete[] m_buffer;
  m_buffer = new Buffer_type[static_cast<unsigned int>(capacity)];
  m_capacity = capacity;
  m_size = size;
  memcpy(&m_buffer[offset], data, static_cast<size_t>(m_size));
}

// ============================================================ //

template <typename Buffer_type>
void Buffer<Buffer_type>::move_set(Buffer_type* data, u64 capacity, u64 size) {
  delete[] m_buffer;
  m_buffer = data;
  m_capacity = capacity;
  m_size = size;
}

// ============================================================ //

template <typename Buffer_type>
std::string Buffer<Buffer_type>::as_string() const {
  return std::string(reinterpret_cast<char8*>(m_buffer), static_cast<size_t>(m_size));
}

// ============================================================ //

template <typename Buffer_type>
void Buffer<Buffer_type>::from_string(const std::string& string) {
  if (m_capacity < string.size()) {
    delete[] m_buffer;
    m_capacity = string.size();
    m_buffer = new Buffer_type[m_capacity];
  }

  m_size = string.size();
  memcpy(m_buffer, string.c_str(), m_size);
}

// ============================================================ //

template <typename Buffer_type>
Buffer_type& Buffer<Buffer_type>::operator[](u64 index) {
  return m_buffer[index];
}

#endif //LIGHTCTRL_BACKEND_BUFFER_HPP
