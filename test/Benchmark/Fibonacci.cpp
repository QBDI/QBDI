/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2023 Quarkslab
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

#include <QBDI.h>

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

QBDI_NOINLINE QBDI::rword Fibonacci(QBDI::rword number) {
  if (number < 2)
    return 1;
  return Fibonacci(number - 1) + Fibonacci(number - 2);
}

static QBDI::VMAction eventCB(QBDI::VMInstanceRef vm,
                              const QBDI::VMState *vmState,
                              QBDI::GPRState *gprState,
                              QBDI::FPRState *fprState, void *data) {
  return QBDI::VMAction::CONTINUE;
}

static QBDI::VMAction eventMemoryCB(QBDI::VMInstanceRef vm,
                                    const QBDI::VMState *vmState,
                                    QBDI::GPRState *gprState,
                                    QBDI::FPRState *fprState, void *data) {
  unsigned *v = static_cast<unsigned *>(data);
  *v += vm->getBBMemoryAccess().size();
  return QBDI::VMAction::CONTINUE;
}

static QBDI::VMAction instEmptyCB(QBDI::VMInstanceRef vm,
                                  QBDI::GPRState *gprState,
                                  QBDI::FPRState *fprState, void *data) {
  return QBDI::VMAction::CONTINUE;
}

static QBDI::VMAction instCB(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState,
                             QBDI::FPRState *fprState, void *data) {
  unsigned *v = static_cast<unsigned *>(data);
  const QBDI::InstAnalysis *instAnalysis =
      vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION |
                          QBDI::ANALYSIS_DISASSEMBLY | QBDI::ANALYSIS_OPERANDS);
  *v += instAnalysis->instSize;

  return QBDI::VMAction::CONTINUE;
}

static QBDI::VMAction instMemoryCB(QBDI::VMInstanceRef vm,
                                   QBDI::GPRState *gprState,
                                   QBDI::FPRState *fprState, void *data) {
  unsigned *v = static_cast<unsigned *>(data);
  *v += vm->getInstMemoryAccess().size();

  return QBDI::VMAction::CONTINUE;
}

TEST_CASE("Benchmark_Fibonacci") {

  BENCHMARK("Fibonacci(2)") { return Fibonacci(2); };

  BENCHMARK_ADVANCED("Fibonacci(2) with QBDI uncached")
  (Catch::Benchmark::Chronometer meter) {
    // init QBDI
    QBDI::VM vm;
    uint8_t *fakestack = nullptr;

    // alloc stack
    QBDI::allocateVirtualStack(vm.getGPRState(), 1 << 20, &fakestack);

    // instrument QBDI
    vm.addInstrumentedModuleFromAddr(reinterpret_cast<QBDI::rword>(Fibonacci));

    meter.measure([&] {
      vm.clearAllCache();
      QBDI::rword ret_value = 0;
      vm.call(&ret_value, reinterpret_cast<QBDI::rword>(Fibonacci),
              {static_cast<QBDI::rword>(2)});
      return ret_value;
    });
    QBDI::alignedFree(fakestack);
  };

  BENCHMARK("Fibonacci(20)") { return Fibonacci(20); };

  BENCHMARK_ADVANCED("Fibonacci(20) with QBDI")
  (Catch::Benchmark::Chronometer meter) {
    // init QBDI
    QBDI::VM vm;
    uint8_t *fakestack = nullptr;

    // alloc stack
    QBDI::allocateVirtualStack(vm.getGPRState(), 1 << 20, &fakestack);

    // instrument QBDI
    vm.addInstrumentedModuleFromAddr(reinterpret_cast<QBDI::rword>(Fibonacci));

    meter.measure([&] {
      QBDI::rword ret_value = 0;
      vm.call(&ret_value, reinterpret_cast<QBDI::rword>(Fibonacci),
              {static_cast<QBDI::rword>(20)});
      return ret_value;
    });
    QBDI::alignedFree(fakestack);
  };

  BENCHMARK_ADVANCED("Fibonacci(20) with QBDI uncached")
  (Catch::Benchmark::Chronometer meter) {
    // init QBDI
    QBDI::VM vm;
    uint8_t *fakestack = nullptr;

    // alloc stack
    QBDI::allocateVirtualStack(vm.getGPRState(), 1 << 20, &fakestack);

    // instrument QBDI
    vm.addInstrumentedModuleFromAddr(reinterpret_cast<QBDI::rword>(Fibonacci));

    meter.measure([&] {
      vm.clearAllCache();
      QBDI::rword ret_value = 0;
      vm.call(&ret_value, reinterpret_cast<QBDI::rword>(Fibonacci),
              {static_cast<QBDI::rword>(20)});
      return ret_value;
    });
    QBDI::alignedFree(fakestack);
  };

  BENCHMARK_ADVANCED("Fibonacci(20) with QBDI uncached with VMEvent")
  (Catch::Benchmark::Chronometer meter) {
    // init QBDI
    QBDI::VM vm;
    uint8_t *fakestack = nullptr;

    // alloc stack
    QBDI::allocateVirtualStack(vm.getGPRState(), 1 << 20, &fakestack);

    // instrument QBDI
    vm.addInstrumentedModuleFromAddr(reinterpret_cast<QBDI::rword>(Fibonacci));

    // add vm event
    vm.addVMEventCB(QBDI::SEQUENCE_EXIT, eventCB, nullptr);

    meter.measure([&] {
      vm.clearAllCache();
      QBDI::rword ret_value = 0;
      vm.call(&ret_value, reinterpret_cast<QBDI::rword>(Fibonacci),
              {static_cast<QBDI::rword>(20)});
      return ret_value;
    });
    QBDI::alignedFree(fakestack);
  };

  BENCHMARK_ADVANCED("Fibonacci(20) with QBDI uncached with InstCallback")
  (Catch::Benchmark::Chronometer meter) {
    // init QBDI
    QBDI::VM vm;
    uint8_t *fakestack = nullptr;

    // alloc stack
    QBDI::allocateVirtualStack(vm.getGPRState(), 1 << 20, &fakestack);

    // instrument QBDI
    vm.addInstrumentedModuleFromAddr(reinterpret_cast<QBDI::rword>(Fibonacci));

    // add callback
    vm.addCodeCB(QBDI::PREINST, instEmptyCB, nullptr);

    meter.measure([&] {
      vm.clearAllCache();
      QBDI::rword ret_value = 0;
      vm.call(&ret_value, reinterpret_cast<QBDI::rword>(Fibonacci),
              {static_cast<QBDI::rword>(20)});
      return ret_value;
    });
    QBDI::alignedFree(fakestack);
  };

  BENCHMARK_ADVANCED(
      "Fibonacci(20) with QBDI uncached with InstCallback and InstAnalysis")
  (Catch::Benchmark::Chronometer meter) {
    // init QBDI
    QBDI::VM vm;
    uint8_t *fakestack = nullptr;

    // alloc stack
    QBDI::allocateVirtualStack(vm.getGPRState(), 1 << 20, &fakestack);

    // instrument QBDI
    vm.addInstrumentedModuleFromAddr(reinterpret_cast<QBDI::rword>(Fibonacci));

    // add callback
    unsigned v = 0;
    vm.addCodeCB(QBDI::PREINST, instCB, &v);

    meter.measure([&] {
      vm.clearAllCache();
      QBDI::rword ret_value = 0;
      vm.call(&ret_value, reinterpret_cast<QBDI::rword>(Fibonacci),
              {static_cast<QBDI::rword>(20)});
      return ret_value;
    });
    QBDI::alignedFree(fakestack);
  };

  BENCHMARK_ADVANCED("Fibonacci(20) with QBDI uncached with MemoryAccess")
  (Catch::Benchmark::Chronometer meter) {
    // init QBDI
    QBDI::VM vm;
    uint8_t *fakestack = nullptr;

    // alloc stack
    QBDI::allocateVirtualStack(vm.getGPRState(), 1 << 20, &fakestack);

    // instrument QBDI
    vm.addInstrumentedModuleFromAddr(reinterpret_cast<QBDI::rword>(Fibonacci));

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

    meter.measure([&] {
      vm.clearAllCache();
      QBDI::rword ret_value = 0;
      vm.call(&ret_value, reinterpret_cast<QBDI::rword>(Fibonacci),
              {static_cast<QBDI::rword>(20)});
      return ret_value;
    });
    QBDI::alignedFree(fakestack);
  };

  BENCHMARK_ADVANCED(
      "Fibonacci(20) with QBDI uncached with MemoryAccess and "
      "getBBMemoryAccess")
  (Catch::Benchmark::Chronometer meter) {
    // init QBDI
    QBDI::VM vm;
    uint8_t *fakestack = nullptr;

    // alloc stack
    QBDI::allocateVirtualStack(vm.getGPRState(), 1 << 20, &fakestack);

    // instrument QBDI
    vm.addInstrumentedModuleFromAddr(reinterpret_cast<QBDI::rword>(Fibonacci));

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

    // add callback
    unsigned v = 0;
    vm.addVMEventCB(QBDI::SEQUENCE_EXIT, eventMemoryCB, &v);

    meter.measure([&] {
      vm.clearAllCache();
      QBDI::rword ret_value = 0;
      vm.call(&ret_value, reinterpret_cast<QBDI::rword>(Fibonacci),
              {static_cast<QBDI::rword>(20)});
      return ret_value;
    });
    QBDI::alignedFree(fakestack);
  };

  BENCHMARK_ADVANCED(
      "Fibonacci(20) with QBDI uncached with MemoryAccess and MemoryCallback")
  (Catch::Benchmark::Chronometer meter) {
    // init QBDI
    QBDI::VM vm;
    uint8_t *fakestack = nullptr;

    // alloc stack
    QBDI::allocateVirtualStack(vm.getGPRState(), 1 << 20, &fakestack);

    // instrument QBDI
    vm.addInstrumentedModuleFromAddr(reinterpret_cast<QBDI::rword>(Fibonacci));

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

    // add callback
    unsigned v = 0;
    vm.addMemAccessCB(QBDI::MEMORY_READ_WRITE, instMemoryCB, &v);

    meter.measure([&] {
      vm.clearAllCache();
      QBDI::rword ret_value = 0;
      vm.call(&ret_value, reinterpret_cast<QBDI::rword>(Fibonacci),
              {static_cast<QBDI::rword>(20)});
      return ret_value;
    });
    QBDI::alignedFree(fakestack);
  };
}
