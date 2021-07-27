/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2021 Quarkslab
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <QBDI/Logs.h>

#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

int main(int argc, char **argv) {

  srand(time(nullptr));

  if (getenv("TEST_DEBUG") != nullptr) {
    QBDI::setLogPriority(QBDI::LogPriority::DEBUG);
  }
  setvbuf(stdout, nullptr, _IONBF, 0);
  setvbuf(stderr, nullptr, _IONBF, 0);

  return Catch::Session().run(argc, argv);
  ;
}
