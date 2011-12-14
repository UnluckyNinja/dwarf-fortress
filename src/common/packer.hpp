/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#ifndef DWARF_FORTRESS_PACKER_HPP_
#define DWARF_FORTRESS_PACKER_HPP_

#include <cstdint>
#include <vector>

#include <iostream>
#include <iomanip>

namespace df {

  typedef std::vector< std::uint8_t > bytes;

  static void debug(df::bytes& bytes) {
    std::clog << "[" << bytes.size() << "] ";
    for (std::uint8_t& b : bytes) {
      std::clog << std::hex << std::setw(2) << std::setfill('0') << static_cast< std::uint32_t >(b) << " ";
    }
    std::clog << std::dec << "\n";
  }

  /**
   * Default stupid versions, simply copy the value bytes to and from the byte array.
   */
  template< typename T >
  struct packer {
      static inline void pack(df::bytes& bytes, T const& value) {
        bytes.insert(bytes.end(), &value, &value + sizeof(value));
      }
      static inline void unpack(df::bytes& bytes, T& value) {
        df::bytes::iterator const start = bytes.end() - sizeof(value);

        value = *reinterpret_cast< T* >(start.base());
        bytes.erase(start, bytes.end());
      }
  };

  template< typename T >
  static inline void pack(df::bytes& bytes, T const& value) {
    packer< T >::pack(bytes, value);
  }
  template< typename T >
  static inline void unpack(df::bytes& bytes, T& value) {
    packer< T >::unpack(bytes, value);
  }

  namespace compression {

    enum type: std::uint8_t {
      none, zlib, zlib_fast, zlib_best
    };

    std::size_t estimate(df::bytes const& bytes, df::compression::type const compression);
    void compress(df::bytes& bytes, df::compression::type const compression);
    void decompress(df::bytes& bytes);

  } // namespace compression

} // namespace df

#endif /* DWARF_FORTRESS_PACKER_HPP_ */
