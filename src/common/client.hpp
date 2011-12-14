/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#ifndef DWARF_FORTRESS_CLIENT_HPP_
#define DWARF_FORTRESS_CLIENT_HPP_

#include <cstdint>
#include <string>

#include <boost/date_time.hpp>
#include <zmq.hpp>

namespace df {

  struct client_configuration_t {
      std::string connection_protocol;
      std::string server_name;

      std::string session_port;
      std::string display_port;
      std::string input_port;

      std::string client_uuid;
      std::string password_hash;

      boost::posix_time::time_duration timeout;
      std::uint32_t retries_count;

      client_configuration_t() :
          timeout(boost::posix_time::seconds(3)), retries_count(3) {
      }
  };

} // namespace df

#endif /* DWARF_FORTRESS_CLIENT_HPP_ */
