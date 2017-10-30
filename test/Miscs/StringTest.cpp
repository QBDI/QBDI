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
#include <gtest/gtest.h>

#include "Utility/String.h"


TEST(startsWithTest, SimpleMatches){
    EXPECT_TRUE(QBDI::String::startsWith("JMP", "JMP64"));
    EXPECT_FALSE(QBDI::String::startsWith("xMP", "JMP"));
    EXPECT_FALSE(QBDI::String::startsWith("JMP64", "x"));
}


TEST(startsWithTest, NullPointers){
    EXPECT_FALSE(QBDI::String::startsWith("JMP64", NULL));
    EXPECT_FALSE(QBDI::String::startsWith(NULL, "x"));
    EXPECT_FALSE(QBDI::String::startsWith(NULL, NULL));
}


TEST(startsWithTest, WildCardBasic){
    EXPECT_TRUE(QBDI::String::startsWith("J*", "JMP"));
    EXPECT_FALSE(QBDI::String::startsWith("J*", "xMP"));
}


TEST(startsWithTest, WildCardAdvanced){
    EXPECT_TRUE(QBDI::String::startsWith("J*P", "JMP"));
    EXPECT_FALSE(QBDI::String::startsWith("J*P", "JMx"));
    EXPECT_TRUE(QBDI::String::startsWith("JMP*", "JMP"));
    EXPECT_TRUE(QBDI::String::startsWith("*", ""));
    EXPECT_TRUE(QBDI::String::startsWith("*", "JMP"));
}


TEST(startsWithTest, Prefix){
    EXPECT_TRUE(QBDI::String::startsWith("B", "B64"));
    EXPECT_TRUE(QBDI::String::startsWith("B*", "B64"));
    EXPECT_FALSE(QBDI::String::startsWith("B", "BIQ"));
    EXPECT_TRUE(QBDI::String::startsWith("B*", "BIQ"));
}
