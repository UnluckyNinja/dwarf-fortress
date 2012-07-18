/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#define BOOST_TEST_MODULE dwarf_fortress_packer_tests
#include <boost/test/unit_test.hpp>

#include <iostream>
#include <iomanip>

#include <zlib.h>

#include "packer.hpp"
#include "varint.hpp"

BOOST_AUTO_TEST_SUITE(packer_tests)

BOOST_AUTO_TEST_CASE(test_packer) {
  df::bytes data;
  df::pack(data, 1);
  df::pack(data, 2);
  df::pack(data, 0xFFFFFF);

  df::debug(data);

  df::compression::compress(data, df::compression::zlib);
  df::debug(data);
  df::compression::decompress(data);
  df::debug(data);

  std::uint32_t value1;
  std::uint32_t value2;
  std::uint32_t value3;
  df::unpack(data, value3);
  df::unpack(data, value2);
  df::unpack(data, value1);

  BOOST_CHECK_EQUAL(value3, 0xFFFFFF);
  BOOST_CHECK_EQUAL(value2, 2);
  BOOST_CHECK_EQUAL(value1, 1);
}

BOOST_AUTO_TEST_CASE(test_packer_string) {
  df::bytes data;
  df::pack(data, "blabla");
  df::debug(data);

  df::compression::compress(data, df::compression::zlib);
  df::debug(data);
  df::compression::decompress(data);
  df::debug(data);

  std::string value1;
  df::unpack(data, value1);

  BOOST_CHECK_EQUAL(value1, "blabla");
}

BOOST_AUTO_TEST_SUITE_END()
