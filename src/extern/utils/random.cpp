/**
 * @file
 * @date 9 juin 2010
 * @todo comment
 */

#include <limits>

#include "extern/utils/random.hpp"
#include "intern/utils/random.hpp"

#include "hidden/global.hpp"

uint32_t mt_trandom() {
  return get_random_uint();
}

int32_t basic_random(int32_t max_value) {
  return mt_trandom() % max_value;
}

void pop_trandom_uniform_seed() {
  mt_cur_buffer--;
  mt_virtual_buffer--;

  pop_random_generator();
}

void push_trandom_triple_seed(uint32_t seed1, uint32_t seed2, uint32_t seed3) {
  push_trandom_uniform_seed(seed1 / 3 + seed2 / 3 + seed3 / 3);
}

void push_trandom_uniform_seed(uint32_t seed) {
  mt_cur_buffer++;
  mt_virtual_buffer++;

  push_random_generator(seed);
}

void r_num() {
  basic_seed = basic_random(::std::numeric_limits< int32_t >::max());
}

void mt_init() {
  mt_cur_buffer = 0;
  mt_virtual_buffer = 0;

  init_random_generator();
}
