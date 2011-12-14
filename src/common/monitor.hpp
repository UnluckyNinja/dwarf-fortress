/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#ifndef DWARF_FORTRESS_MONITOR_HPP_
#define DWARF_FORTRESS_MONITOR_HPP_

#include <cstdint>

#include <boost/chrono.hpp>

namespace df {

  struct monitor {
      static monitor instance;

      std::uint64_t bytes_sent;
      std::uint64_t messages_sent;

      std::uint64_t bytes_compressed;
      std::uint64_t bytes_compressed_result;
      std::uint64_t data_compressed;
      boost::chrono::high_resolution_clock::time_point start_time;

      monitor();
      ~monitor();

      void sent(std::size_t size);
      void compress(std::size_t size, std::size_t compressed_size);
  };

} // namespace df

#endif /* DWARF_FORTRESS_MONITOR_HPP_ */
