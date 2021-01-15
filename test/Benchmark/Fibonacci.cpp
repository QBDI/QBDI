/*
 * This file is part of QBDI.
 *
 * Copyright 2017 Quarkslab
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

TEST_CASE("Benchmark_Fibonacci") {

    BENCHMARK("Fibonacci(25)") {
        return Fibonacci(25);
    };

    BENCHMARK_ADVANCED("Fibonacci(25) with QBDI")(Catch::Benchmark::Chronometer meter) {
        // init QBDI
        QBDI::VM vm;
        uint8_t *fakestack = nullptr;

        // alloc stack
        QBDI::allocateVirtualStack(vm.getGPRState(), 1<<20, &fakestack);

        // instrument QBDI
        vm.addInstrumentedModuleFromAddr(reinterpret_cast<QBDI::rword>(Fibonacci));

        meter.measure(
            [&] {
                QBDI::rword ret_value = 0;
                vm.call(&ret_value, reinterpret_cast<QBDI::rword>(Fibonacci), {static_cast<QBDI::rword>(25)});
                return ret_value;
            });
        QBDI::alignedFree(fakestack);
    };
}
