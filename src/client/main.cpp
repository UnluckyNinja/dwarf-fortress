/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#include <locale>
#include <iostream>

#include <boost/thread.hpp>

#include "signals.hpp"
#include "client/session.hpp"
#include "client/display.hpp"
#include "client/input.hpp"

int main(int argc, char** argv) {
  df::add_kill_handler();
  zmq::context_t zmq_context(1);

  std::string server_name = std::string(argv[1]);

  df::client_configuration_t client_config;
  client_config.client_uuid = "<client-uuid>";
  client_config.password_hash = "<password-hash>";
  client_config.server_name = server_name;
  client_config.connection_protocol = "tcp";
  client_config.display_port = "6000";
  client_config.input_port = "6001";
  client_config.session_port = "6002";

  df::session_client session_client(client_config);
  df::display_client display_client(client_config);
  df::input_client input_client(client_config);
  boost::thread session_thread = boost::thread(df::session_client::control_thread(session_client, zmq_context));
  boost::thread display_thread = boost::thread(df::display_client::control_thread(display_client, zmq_context));
  boost::thread input_thread = boost::thread(df::input_client::control_thread(input_client, zmq_context));

#ifdef unix
  setlocale(LC_ALL, "");
#endif

  session_thread.join();
  display_thread.join();
  input_thread.join();

  return 0;
}
