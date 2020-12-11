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
#include <stdio.h>
#include <cstdint>

#include <catch2/catch.hpp>
#include "Memory.hpp"

TEST_CASE("AllocAlignedTest, CorrectSize") {
    const static size_t size = 1000;
    uint8_t *array =  (uint8_t*) QBDI::alignedAlloc(size, sizeof(void*));
    REQUIRE(array != nullptr);
    for(size_t i = 0; i < size; i++) {
        array[i] = 0x42;
    }
    QBDI::alignedFree(array);
    SUCCEED();
}

TEST_CASE("AllocAlignedTest, CorrectAlignement") {
    const static size_t size = 1000;
    const static size_t align = 16;
    void *array =  QBDI::alignedAlloc(size, align);
    REQUIRE(array != nullptr);
    REQUIRE(reinterpret_cast<std::uintptr_t>(array) % align == static_cast<std::uintptr_t>(0));
    QBDI::alignedFree(array);
    SUCCEED();
}
