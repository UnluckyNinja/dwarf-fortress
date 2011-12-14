/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#include "monitor.hpp"

namespace df {

  monitor monitor::instance;

  monitor::monitor() :
      bytes_sent(0), messages_sent(0), bytes_compressed(0), bytes_compressed_result(0), data_compressed(0), start_time(boost::chrono::high_resolution_clock::now()) {
  }
  monitor::~monitor() {
    boost::chrono::high_resolution_clock::time_point end_time = boost::chrono::high_resolution_clock::now();
    boost::chrono::nanoseconds duration = end_time - start_time;

    std::clog << "total time: " << duration << "\n";
    std::clog << "total bytes sent: " << bytes_sent << "\n";
    std::clog << "total messages sent: " << messages_sent << "\n";
    std::clog << "average bytes per message: " << (static_cast< double >(bytes_sent) / messages_sent) << "\n";
    std::clog << "messages per second: " << (static_cast< double >(messages_sent) * 1000000000 / duration.count())
        << "\n";
    std::clog << "kbytes per second: " << (static_cast< double >(bytes_sent) * 1000000000 / 1024 / duration.count())
        << "\n";

    std::clog << "total data compressed: " << data_compressed << "\n";
    std::clog << "total bytes compressed: " << bytes_compressed << "\n";
    std::clog << "total bytes compressed result: " << bytes_compressed_result << "\n";
    std::clog << "compression ratio: " << (static_cast< double >(bytes_compressed_result) * 100 / bytes_compressed)
        << "\n";
  }

  void monitor::sent(std::size_t size) {
    bytes_sent += size;
    messages_sent++;
  }

  void monitor::compress(std::size_t size, std::size_t compressed_size) {
    bytes_compressed += size;
    bytes_compressed_result += compressed_size;
    data_compressed++;
  }

} // namespace df
