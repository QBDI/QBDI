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
#include <stdlib.h>
#include <gtest/gtest.h>
#include <vector>

#include "Range.h"

TEST(Range, StateIntegrity) {
    static const int N = 100;
    std::vector<QBDI::Range<int>> testRanges;
    QBDI::RangeSet<int> rangeSet;

    for(int i = 0; i < N; i++) {
        int start = rand()%900;
        int end = start + rand()%100 + 1;
        int delta = -rangeSet.size();
        QBDI::Range<int> r(start, end);

        testRanges.push_back(r);
        rangeSet.add(r);
        delta += rangeSet.size();

        EXPECT_GE(r.size(), delta);
        EXPECT_TRUE(rangeSet.contains(r));
        EXPECT_TRUE(rangeSet.contains(r.start));
        EXPECT_TRUE(rangeSet.contains(r.end - 1));
    }

    for(int i = 0; i < N; i++) {
        int delta = rangeSet.size();
        QBDI::Range<int> r = testRanges.back();

        testRanges.pop_back();
        rangeSet.remove(r);
        delta -= rangeSet.size();

        EXPECT_GE(r.size(), delta);
        EXPECT_FALSE(rangeSet.contains(r));
        EXPECT_FALSE(rangeSet.contains(r.start));
        EXPECT_FALSE(rangeSet.contains(r.end - 1));
    }
    EXPECT_EQ(0, rangeSet.size());
}

template<typename T> void randomPermutation(std::vector<T> &v) {
    size_t i = 0;
    for(i = 0; i < v.size(); i++) {
        size_t p = rand() % v.size();
        T t = v[i];
        v[i] = v[p];
        v[p] = t;
    }
}

TEST(Range, Commutativity) {
    static const int N = 100;
    std::vector<QBDI::Range<int>> testRanges;
    QBDI::RangeSet<int> rangeSet;

    for(int i = 0; i < N; i++) {
        int start = rand() % 1000000;
        int end = start + 1 + rand() % 10000;
        QBDI::Range<int> r(start, end);
        testRanges.push_back(r);
        rangeSet.add(r);
    }

    for(int c = 0; c < N; c++) {
        QBDI::RangeSet<int> permutedRangeSet;
         
        randomPermutation(testRanges);
        for(int i = 0; i < N; i++) {
            permutedRangeSet.add(testRanges[i]);
        }
        ASSERT_EQ(rangeSet.size(), permutedRangeSet.size());
        for(size_t i = 0; i < rangeSet.getRanges().size(); i++) {
            ASSERT_EQ(rangeSet.getRanges()[i].start, permutedRangeSet.getRanges()[i].start);
            ASSERT_EQ(rangeSet.getRanges()[i].end, permutedRangeSet.getRanges()[i].end);
        }
    }
}

TEST(Range, Intersection) {
    static const int N = 100;
    QBDI::RangeSet<int> rangeSet1;
    QBDI::RangeSet<int> rangeSet2;
    QBDI::RangeSet<int> intersection1;
    QBDI::RangeSet<int> intersection2;

    for(int i = 0; i < N; i++) {
        int start = rand() % 1000000;
        int end = start + 1 + rand() % 10000;
        rangeSet1.add(QBDI::Range<int>(start, end));
    }

    for(int i = 0; i < N; i++) {
        int start = rand() % 1000000;
        int end = start + 1 + rand() % 10000;
        rangeSet2.add(QBDI::Range<int>(start, end));
    }

    intersection1.add(rangeSet1);
    intersection1.intersect(rangeSet2);
    intersection2.add(rangeSet2);
    intersection2.intersect(rangeSet1);

    ASSERT_EQ(intersection1, intersection2);

    for(QBDI::Range<int> r: intersection1.getRanges()) {
        ASSERT_EQ(true, rangeSet1.contains(r));
        ASSERT_EQ(true, rangeSet2.contains(r));
    }
}
