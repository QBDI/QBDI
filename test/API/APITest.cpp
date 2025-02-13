/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2025 Quarkslab
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
#include "APITest.h"
#include <catch2/catch_test_macros.hpp>

#include <sstream>
#include <string>
#include "inttypes.h"

#include "TestSetup/InMemoryAssembler.h"

#include "QBDI/Memory.hpp"
#include "QBDI/Platform.h"
#include "QBDI/Range.h"

#define STACK_SIZE 4096

static void dummyFn() {}

APITest::APITest() : vm() {

  bool instrumented = vm.addInstrumentedModuleFromAddr((QBDI::rword)&dummyFn);
  REQUIRE(instrumented);

  // get GPR state
  state = vm.getGPRState();

  // Get a pointer to the GPR state of the vm
  // Setup initial GPR state, this fakestack will produce a ret NULL at the end
  // of the execution
  bool ret = QBDI::allocateVirtualStack(state, STACK_SIZE, &fakestack);
  REQUIRE(ret == true);
}

APITest::~APITest() {
  QBDI::alignedFree(fakestack);
  objects.clear();
}

QBDI::rword APITest::genASM(const char *source, QBDI::CPUMode cpuMode,
                            const std::vector<std::string> mattrs) {
  std::ostringstream finalSource;

#ifdef QBDI_ARCH_ARM
  if (cpuMode == QBDI::CPUMode::Thumb) {
    finalSource << ".thumb\n";
  }
  finalSource << source << "\n"
              << "bx lr\n";
  if (cpuMode == QBDI::CPUMode::ARM) {
    objects.emplace_back(finalSource.str().c_str(), "", "arm", mattrs);
  } else {
    objects.emplace_back(finalSource.str().c_str(), "", "thumb", mattrs);
  }
#else
  finalSource << source << "\n"
              << "ret\n";
  if constexpr (QBDI::is_aarch64) {
    objects.emplace_back(finalSource.str().c_str(), "", "aarch64", mattrs);
  } else {
    objects.emplace_back(finalSource.str().c_str(), "", "", mattrs);
  }
#endif

  const llvm::ArrayRef<uint8_t> &code = objects.back().getCode();
  QBDI::rword codeAddr = (QBDI::rword)code.data();
  llvm::sys::Memory::InvalidateInstructionCache(code.data(), code.size());

  vm.addInstrumentedRange(codeAddr, codeAddr + code.size());
  vm.clearCache(codeAddr, codeAddr + code.size());

#ifdef QBDI_ARCH_ARM
  if (cpuMode == QBDI::CPUMode::Thumb) {
    codeAddr |= 1;
  }
#endif

  vm.precacheBasicBlock(codeAddr);

  return codeAddr;
}

bool APITest::runOnASM(QBDI::rword *retval, const char *source,
                       const std::vector<QBDI::rword> &args,
                       QBDI::CPUMode cpuMode,
                       const std::vector<std::string> mattrs) {
  QBDI::rword addr = genASM(source, cpuMode, mattrs);

  return vm.call(retval, addr, args);
}
