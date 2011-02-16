/**
 * @file
 * @date 14 juin 2010
 * @todo comment
 */

#include <boost/program_options.hpp>

#include "extern/command_line.hpp"
#include "intern/config.hpp"

#include <iostream>

command_linest::command_linest() :
  gen_id(-1), use_seed(false), use_param(false) {
}

void command_linest::init(const ::std::string& str) {
  original = str;

  world_config const& world = config::instance().world();
  if (world.generate) {
    gen_id = world.id;
    world_seed = world.seed;
    use_seed = world.use_seed;
    world_param = world.parameter;
    use_param = world.use_parameter;
  }
}
