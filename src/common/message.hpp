/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#ifndef DWARF_FORTRESS_MESSAGE_HPP_
#define DWARF_FORTRESS_MESSAGE_HPP_

#include <cstdint>
#include <string>
#include <exception>

#include <zmq.hpp>

#include "packer.hpp"
#include "varint.hpp"
#include "message/head.hpp"

#include "monitor.hpp"

namespace df {

  template< typename MessageType >
  static void subscribe(zmq::socket_t& socket, MessageType const&) {
    df::bytes bytes;
    df::pack(bytes, df::message::get_message_type< MessageType >::value);

    socket.setsockopt(ZMQ_SUBSCRIBE, bytes.data(), bytes.size());
  }

  template< typename MessageType >
  static void subscribe(zmq::socket_t& socket, std::string& peer_uuid, MessageType const&) {
    df::message::head_t message;
    message.type = df::message::get_message_type< MessageType >::value;
    message.peer_uuid = peer_uuid;

    df::bytes bytes;
    df::pack(bytes, message);

    socket.setsockopt(ZMQ_SUBSCRIBE, bytes.data(), bytes.size());
  }

  static bool send(zmq::socket_t& socket, df::bytes const& bytes, bool blocking = false, bool send_more = false) {
    zmq::message_t message(bytes.size());
    memcpy(message.data(), bytes.data(), bytes.size());

    std::uint32_t flags = 0;
    flags |= blocking ? 0 : ZMQ_NOBLOCK;
    flags |= send_more ? ZMQ_SNDMORE : 0;

    return socket.send(message, flags);
  }

  static bool receive(zmq::socket_t& socket, df::bytes& bytes, bool blocking = false) {
    zmq::message_t message;

    std::uint32_t flags = 0;
    flags |= blocking ? 0 : ZMQ_NOBLOCK;

    if (!socket.recv(&message, flags)) {
      return false;
    }

    std::uint8_t* const data = reinterpret_cast< std::uint8_t* >(message.data());
    bytes.assign(data, data + message.size());
    return true;
  }

  template< typename MessageType >
  static bool send(zmq::socket_t& socket, std::string const& peer_uuid, MessageType const& message) {
    std::size_t bytes_sent = 0;

    {
      df::message::head_t message_head;
      message_head.type = df::message::get_message_type< MessageType >::value;
      message_head.peer_uuid = peer_uuid;

      df::bytes bytes;
      df::pack(bytes, message_head);

      if (!df::send(socket, bytes, false, true)) {
        return false;
      }

      bytes_sent += bytes.size();
    }

    {
      df::bytes bytes;
      df::pack(bytes, message);

      std::size_t uncompressed_size = bytes.size();
      if (df::compression::estimate(bytes, df::compression::zlib) < bytes.size()) {
        df::compression::compress(bytes, df::compression::type::zlib);
      } else {
        df::compression::compress(bytes, df::compression::type::none);
      }
      monitor::instance.compress(uncompressed_size, bytes.size());

      df::send(socket, bytes, true);
      bytes_sent += bytes.size();
    }

    monitor::instance.sent(bytes_sent);
    return true;
  }

  struct unexpected_message_type: std::exception {
  };

  template< typename MessageType >
  static bool receive(zmq::socket_t& socket, std::string& peer_uuid, MessageType& message) {
    {
      df::bytes bytes;
      if (!df::receive(socket, bytes)) {
        return false;
      }

      df::message::head_t message_head;
      df::unpack(bytes, message_head);

      peer_uuid = message_head.peer_uuid;
      if (message_head.type != df::message::get_message_type< MessageType >::value) {
        throw unexpected_message_type();
      }
    }

    {
      df::bytes bytes;
      df::receive(socket, bytes, true);

      df::compression::decompress(bytes);
      df::unpack(bytes, message);
    }

    return true;
  }

} // namespace df

#endif /* DWARF_FORTRESS_MESSAGE_HPP_ */
