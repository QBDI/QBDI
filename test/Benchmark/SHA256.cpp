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
#include "sha256.h"

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

static const uint8_t buffer[1<<20] = {0};

QBDI_NOINLINE sha256::HashType* compute_sha() {
    sha256::HashType* hash = new sha256::HashType();
    *hash = sha256::compute(buffer, sizeof(buffer));
    return hash;
}

TEST_CASE("Benchmark sha256") {

    BENCHMARK("sha256(len: 1MBytes)") {
        return compute_sha();
    };

    BENCHMARK_ADVANCED("sha256(len: 1MBytes) with QBDI")(Catch::Benchmark::Chronometer meter) {
        // init QBDI
        QBDI::VM vm;
        uint8_t *fakestack = nullptr;

        // alloc stack
        QBDI::allocateVirtualStack(vm.getGPRState(), 1<<20, &fakestack);

        // instrument QBDI
        vm.addInstrumentedModuleFromAddr(reinterpret_cast<QBDI::rword>(compute_sha));

        meter.measure(
            [&] {
                QBDI::rword ret_value = 0;
                vm.call(&ret_value, reinterpret_cast<QBDI::rword>(compute_sha), {});
                return ret_value;
            });
        QBDI::alignedFree(fakestack);
    };
}
