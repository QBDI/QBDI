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
#include <gtest/gtest.h>

#include "Memory.hpp"

TEST(AllocAlignedTest, CorrectSize) {
    const static size_t size = 1000;
    uint8_t *array =  (uint8_t*) QBDI::alignedAlloc(size, sizeof(void*));
    ASSERT_NE(nullptr, array);
    for(size_t i = 0; i < size; i++) {
        array[i] = 0x42;
    }
    QBDI::alignedFree(array);
    SUCCEED();
}

TEST(AllocAlignedTest, CorrectAlignement) {
    const static size_t size = 1000;
    const static size_t align = 16;
    void *array =  QBDI::alignedAlloc(size, align);
    ASSERT_NE(nullptr, array);
    ASSERT_EQ((std::uintptr_t) 0, (std::uintptr_t) array % align);
    QBDI::alignedFree(array);
    SUCCEED();
}
