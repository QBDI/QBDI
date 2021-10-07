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

#ifndef QBDITEST_APITEST_H
#define QBDITEST_APITEST_H

#include <memory>
#include <string>
#include <vector>

#include "TestSetup/InMemoryAssembler.h"

#include "QBDI/VM.h"

class APITest {
private:
  std::vector<InMemoryObject> objects;

protected:
  QBDI::GPRState *state;
  uint8_t *fakestack;

public:
  QBDI::VM vm;

  APITest();
  ~APITest();

  QBDI::rword genASM(const char *source,
                     const std::vector<std::string> mattrs = {});

  bool runOnASM(QBDI::rword *retval, const char *source,
                const std::vector<QBDI::rword> &args = {},
                const std::vector<std::string> mattrs = {});
};

#endif /* QBDITEST_APITEST_H */
