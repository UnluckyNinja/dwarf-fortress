/**
 * @file
 * @date 9 juin 2010
 * @todo comment
 */

#ifndef INTERN_UTILS_RANDOM_HPP_
#define INTERN_UTILS_RANDOM_HPP_

#include "integer_types.hpp"

uint32_t get_random_uint();
int32_t get_random_int(uint32_t = 2147483647LU);
void pop_random_generator();
void push_random_generator(uint32_t);
void init_random_generator();

#endif /* INTERN_UTILS_RANDOM_HPP_ */
