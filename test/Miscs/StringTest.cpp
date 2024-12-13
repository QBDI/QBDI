/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2024 Quarkslab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <catch2/catch_test_macros.hpp>

#include "Utility/String.h"

TEST_CASE("startsWithTest-SimpleMatches") {
  CHECK(QBDI::String::startsWith("JMP", "JMP64"));
  CHECK_FALSE(QBDI::String::startsWith("xMP", "JMP"));
  CHECK_FALSE(QBDI::String::startsWith("JMP64", "x"));
}

TEST_CASE("startsWithTest-NullPointers") {
  CHECK_FALSE(QBDI::String::startsWith("JMP64", NULL));
  CHECK_FALSE(QBDI::String::startsWith(NULL, "x"));
  CHECK_FALSE(QBDI::String::startsWith(NULL, NULL));
}

TEST_CASE("startsWithTest-WildCardBasic") {
  CHECK(QBDI::String::startsWith("J*", "JMP"));
  CHECK_FALSE(QBDI::String::startsWith("J*", "xMP"));
}

TEST_CASE("startsWithTest-WildCardAdvanced") {
  CHECK(QBDI::String::startsWith("J*P", "JMP"));
  CHECK_FALSE(QBDI::String::startsWith("J*P", "JMx"));
  CHECK(QBDI::String::startsWith("JMP*", "JMP"));
  CHECK(QBDI::String::startsWith("*", ""));
  CHECK(QBDI::String::startsWith("*", "JMP"));
}

TEST_CASE("startsWithTest-Prefix") {
  CHECK(QBDI::String::startsWith("B", "B64"));
  CHECK(QBDI::String::startsWith("B*", "B64"));
  CHECK_FALSE(QBDI::String::startsWith("B", "BIQ"));
  CHECK(QBDI::String::startsWith("B*", "BIQ"));
}
