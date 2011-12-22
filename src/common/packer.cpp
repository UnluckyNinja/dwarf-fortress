/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#include "packer.hpp"
#include "varint.hpp"

#include <zlib.h>

namespace df {

  namespace compression {

    std::size_t estimate(df::bytes const& bytes, df::compression::type const compression) {
      switch (compression) {
        case df::compression::type::zlib:
        case df::compression::type::zlib_best:
        case df::compression::type::zlib_fast:
          return compressBound(bytes.size()) / 2 + df::vuint64_t(bytes.size()).size_;

        case df::compression::type::none:
        default:
          return bytes.size();
      }
    }

    void compress(df::bytes& bytes, df::compression::type const compression) {
      switch (compression) {
        case df::compression::type::zlib:
        case df::compression::type::zlib_best:
        case df::compression::type::zlib_fast: {

          int level;
          switch (compression) {
            case df::compression::type::zlib_best:
              level = Z_BEST_COMPRESSION;
              break;
            case df::compression::type::zlib_fast:
              level = Z_BEST_SPEED;
              break;
            case df::compression::type::zlib:
            default:
              level = Z_DEFAULT_COMPRESSION;
              break;
          }

          df::bytes uncompressed = bytes;

          bytes.resize(compressBound(uncompressed.size()));
          uLongf compressed_size = bytes.size();
          ::compress2(bytes.data(), &compressed_size, uncompressed.data(), uncompressed.size(), level);
          bytes.resize(compressed_size);

          df::pack(bytes, uncompressed.size());
          break;
        }

        case df::compression::type::none:
        default:
          break;
      }

      df::pack(bytes, compression);
    }

    void decompress(df::bytes& bytes) {
      df::compression::type compression;
      df::unpack(bytes, compression);

      switch (compression) {
        case df::compression::type::zlib:
        case df::compression::type::zlib_best:
        case df::compression::type::zlib_fast: {
          std::size_t uncompressed_size;
          df::unpack(bytes, uncompressed_size);

          df::bytes compressed = bytes;
          bytes.resize(uncompressed_size);

          uLongf zuncompressed_size = uncompressed_size;
          ::uncompress(bytes.data(), &zuncompressed_size, compressed.data(), compressed.size());
          break;
        }

        case df::compression::type::none:
        default:
          break;
      }

    }

  } // namespace compression

} // namespace df
