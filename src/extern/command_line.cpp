/**
 * @file
 * @date 14 juin 2010
 * @todo comment
 */

#include <boost/program_options.hpp>

#include "extern/command_line.hpp"

#include <iostream>

command_linest::command_linest() :
  gen_id(-1), use_seed(false), use_param(false) {
}

void command_linest::init(const ::std::string& str) {
  namespace po = boost::program_options;

  long id;
  unsigned long int seed;
  ::std::string param;

  po::options_description desc("Allowed options");
  desc.add_options()("help,h", "produce help message")("gen,g", "generate world")("id,i", po::value< long >(&id),
      "generation id")("seed,s", po::value< unsigned long int >(&seed), "world seed")("param,p", po::value<
      ::std::string >(&param), "world param");

  original = str;
  ::std::vector< ::std::string > args = po::split_unix(str);

  po::variables_map vm;
  po::store(po::command_line_parser(args). options(desc).run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
    ::std::cout << desc << ::std::endl;
    exit(1);
  } else if (vm.count("gen")) {
    if (vm.count("id")) {
      gen_id = id;

      if (vm.count("seed")) {
        world_seed = seed;
        use_seed = true;
      }

      if (vm.count("param")) {
        world_param = param;
        use_param = true;
      }
    } else {
      ::std::cout << desc << ::std::endl;
      exit(1);
    }
  }
}
