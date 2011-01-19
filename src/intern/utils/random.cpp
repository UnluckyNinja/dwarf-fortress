/**
 * @file
 * @date 9 juin 2010
 * @todo comment
 */

#include "intern/utils/random.hpp"

#include <logging/logging.hpp>

#include <stack>
#include <limits>
#include <boost/random.hpp>

#define WARMING_COUNT 100
typedef ::boost::mt19937 generator_type_t;

static ::std::stack< generator_type_t > generator_stack;
static const ::boost::uniform_int< uint32_t > uint32_distribution(0, ::std::numeric_limits< uint32_t >::max());

uint32_t get_random_uint() {
  if (generator_stack.empty()) {
    init_random_generator();
  }

  ::boost::variate_generator< generator_type_t&, ::boost::uniform_int< uint32_t > > random(generator_stack.top(),
      uint32_distribution);

  return random();
}

int32_t get_random_int(uint32_t max_value) {
  max_value %= ::std::numeric_limits< int32_t >::max();

  return get_random_uint() % max_value;
}

void pop_random_generator() {
  generator_stack.pop();
}

void push_random_generator(uint32_t seed) {
  generator_stack.push(generator_type_t(seed));
}

void init_random_generator() {
  while (!generator_stack.empty()) {
    generator_stack.pop();
  }

  generator_stack.push(generator_type_t());

  for (int i = 0; i < WARMING_COUNT; ++i) {
    get_random_uint();
  }
}
