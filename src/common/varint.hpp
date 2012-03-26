/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#ifndef DWARF_FORTRESS_VARINT_HPP_
#define DWARF_FORTRESS_VARINT_HPP_

#include <cstdint>
#include <vector>
#include <algorithm>
#include <cstring>

#include "packer.hpp"

namespace df {

  namespace detail {

    template< typename IntegerType, std::size_t const BufferSize = sizeof(IntegerType) + sizeof(IntegerType) / 8 + 1 >
    struct varint;

    template< typename IntegerType >
    struct varint< IntegerType, 1 > {
        static std::size_t write(std::uint8_t buffer[1], IntegerType const value, bool matched) {
          *buffer = static_cast< std::uint8_t >(value & 0x7F);
          return 1;
        }
        static std::size_t read(std::uint8_t const buffer[1], IntegerType& value) {
          value = *buffer;
          return 1;
        }
    };

    template< typename IntegerType, bool const Signed >
    struct varint_value {
        IntegerType integer_;
        void set(IntegerType const& integer) { integer_ = integer; }
        IntegerType get() const { return integer_; }
    };

    template< typename IntegerType >
    struct varint_value< IntegerType, true > {
        IntegerType integer_;
        void set(IntegerType const& integer) { integer_ = (integer << 1) ^ (integer >> std::numeric_limits< IntegerType >::digits); }
        IntegerType get() const { return (integer_ >> 1) ^ -static_cast< IntegerType >(integer_ & 1); }
    };

    template< typename IntegerType, std::size_t const BufferSize >
    struct varint {
        typedef varint< IntegerType, BufferSize - 1 > next_vint;
        static std::uint8_t const vint_mask = 0x7F;
        static IntegerType const max_shift = sizeof(IntegerType) * 8;
        static IntegerType const vint_shift = (BufferSize - 1) * 7;

        static std::size_t write(std::uint8_t buffer[BufferSize], IntegerType const value, bool matched = false) {
          std::uint8_t vbyte = static_cast< std::uint8_t >((value >> vint_shift) & vint_mask);
          std::size_t size = 0;
          if (matched || vbyte) {
            *(buffer++) = vbyte | ~(vint_mask);
            matched = true;
            size = 1;
          }

          return size + next_vint::write(buffer, value, matched);
        }

        static std::size_t read(std::uint8_t const buffer[BufferSize], IntegerType& value) {
          std::uint8_t vbyte = *buffer;
          std::size_t size = 1;
          if (vbyte & ~(vint_mask)) {
            size += next_vint::read(buffer + 1, value);
          }

          value |= (vbyte & vint_mask) << (size - 1) * 7;
          return size;
        }

        std::uint8_t buffer_[BufferSize];
        std::size_t size_;
        varint_value< IntegerType, std::numeric_limits< IntegerType >::is_signed > value_;

#ifdef DWARF_FORTRESS_VINT_DISABLE_COMPRESSION
        explicit varint(IntegerType const value) {
          value_.set(value);
          size_ = sizeof(IntegerType);
          memcpy(buffer_, &value_, size_);
        }
        explicit varint(std::uint8_t const* buffer) {
          value_.set(0);
          size_ = sizeof(IntegerType);
          memcpy(&value_, buffer, size_);
          memcpy(buffer_, buffer, size_);
        }
#else /* DWARF_FORTRESS_VINT_DISABLE_COMPRESSION */

        explicit varint(IntegerType const value) {
          value_.set(value);
          size_ = write(buffer_, value_);
        }
        explicit varint(std::uint8_t const* buffer) {
          value_.set(0);
          size_ = read(buffer, value_);
          std::memcpy(buffer_, buffer, size_);
        }
#endif /* DWARF_FORTRESS_VINT_DISABLE_COMPRESSION */

        operator IntegerType() const {
          return value_.get();
        }
    };

  } // namespace detail

  /**
   * Varint pack/unpack implementation.
   *
   * Pack the bytes in reverse order so it will be easy to unpack them without guard.
   *
   * Unpack by popping the bytes from the back, until we have found the last byte of
   * the vint. The last one has its most significant bit set to 0.
   */
#define DECLARE_VARINT(integer_m)                                                 \
  typedef detail::varint< std::integer_m > v##integer_m;                          \
  template< >                                                                   \
  struct packer< std::integer_m > {                                             \
    static inline void pack(df::bytes& bytes, std::integer_m const value) {     \
      df::v##integer_m v(value);                                                \
      bytes.insert(bytes.end(), v.buffer_, v.buffer_ + v.size_);                \
      std::reverse(bytes.end() - v.size_, bytes.end());                         \
    }                                                                           \
    static inline void unpack(df::bytes& bytes, std::integer_m& value) {        \
      df::bytes buffer;                                                         \
      do {                                                                      \
        buffer.push_back(bytes.back());                                         \
        bytes.pop_back();                                                       \
      } while (buffer.back() & ~(df::v##integer_m::vint_mask));                 \
      value = df::v##integer_m(buffer.data());                                  \
    }                                                                           \
  };

  DECLARE_VARINT(int8_t)
  DECLARE_VARINT(int16_t)
  DECLARE_VARINT(int32_t)
  DECLARE_VARINT(int64_t)

  DECLARE_VARINT(uint8_t)
  DECLARE_VARINT(uint16_t)
  DECLARE_VARINT(uint32_t)
  DECLARE_VARINT(uint64_t)

#undef DECLARE_VARINT

  template< >
  void pack< std::string >(df::bytes& bytes, std::string const& value) {
    bytes.insert(bytes.end(), value.begin(), value.end());
    df::pack(bytes, value.size());
  }
  template< >
  void unpack< std::string >(df::bytes& bytes, std::string& value) {
    std::size_t size;
    df::unpack(bytes, size);
    value.assign(bytes.end() - size, bytes.end());
    bytes.erase(bytes.end() - size, bytes.end());
  }

  template< std::size_t const Size >
  struct packer< char[Size] > {
      static inline void pack(df::bytes& bytes, char const value[Size]) {
        df::pack(bytes, std::string(value));
      }
  };

} // namespace df

#endif /* DWARF_FORTRESS_VARINT_HPP_ */
