/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2022 Quarkslab
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
#include <catch2/catch.hpp>
#include <stdlib.h>
#include <vector>

#include "QBDI/Range.h"

TEST_CASE("Range-StateIntegrity") {
  static const int N = 100;
  std::vector<QBDI::Range<int>> testRanges;
  QBDI::RangeSet<int> rangeSet;

  for (int i = 0; i < N; i++) {
    int start = rand() % 900;
    int end = start + rand() % 100 + 1;
    int delta = -rangeSet.size();
    QBDI::Range<int> r(start, end);

    testRanges.push_back(r);
    rangeSet.add(r);
    delta += rangeSet.size();

    CHECK(r.size() >= delta);
    CHECK(rangeSet.contains(r));
    CHECK(rangeSet.contains(r.start()));
    CHECK(rangeSet.contains(r.end() - 1));
  }

  for (int i = 0; i < N; i++) {
    int delta = rangeSet.size();
    QBDI::Range<int> r = testRanges.back();

    testRanges.pop_back();
    rangeSet.remove(r);
    delta -= rangeSet.size();

    CHECK(r.size() >= delta);
    CHECK_FALSE(rangeSet.contains(r));
    CHECK_FALSE(rangeSet.contains(r.start()));
    CHECK_FALSE(rangeSet.contains(r.end() - 1));
  }
  CHECK(0 == rangeSet.size());
}

template <typename T>
void randomPermutation(std::vector<T> &v) {
  size_t i = 0;
  for (i = 0; i < v.size(); i++) {
    size_t p = rand() % v.size();
    T t = v[i];
    v[i] = v[p];
    v[p] = t;
  }
}

TEST_CASE("Range-Commutativity") {
  static const int N = 100;
  std::vector<QBDI::Range<int>> testRanges;
  QBDI::RangeSet<int> rangeSet;

  for (int i = 0; i < N; i++) {
    int start = rand() % 1000000;
    int end = start + 1 + rand() % 10000;
    QBDI::Range<int> r(start, end);
    testRanges.push_back(r);
    rangeSet.add(r);
  }

  for (int c = 0; c < N; c++) {
    QBDI::RangeSet<int> permutedRangeSet;

    randomPermutation(testRanges);
    for (int i = 0; i < N; i++) {
      permutedRangeSet.add(testRanges[i]);
    }
    REQUIRE(rangeSet.size() == permutedRangeSet.size());
    for (size_t i = 0; i < rangeSet.getRanges().size(); i++) {
      REQUIRE(rangeSet.getRanges()[i].start() ==
              permutedRangeSet.getRanges()[i].start());
      REQUIRE(rangeSet.getRanges()[i].end() ==
              permutedRangeSet.getRanges()[i].end());
    }
  }
}

TEST_CASE("Range-Intersection") {
  static const int N = 100;
  QBDI::RangeSet<int> rangeSet1;
  QBDI::RangeSet<int> rangeSet2;
  QBDI::RangeSet<int> intersection1;
  QBDI::RangeSet<int> intersection2;

  for (int i = 0; i < N; i++) {
    int start = rand() % 1000000;
    int end = start + 1 + rand() % 10000;
    rangeSet1.add(QBDI::Range<int>(start, end));
  }

  for (int i = 0; i < N; i++) {
    int start = rand() % 1000000;
    int end = start + 1 + rand() % 10000;
    rangeSet2.add(QBDI::Range<int>(start, end));
  }

  intersection1.add(rangeSet1);
  intersection1.intersect(rangeSet2);
  intersection2.add(rangeSet2);
  intersection2.intersect(rangeSet1);

  REQUIRE(intersection1 == intersection2);

  for (QBDI::Range<int> r : intersection1.getRanges()) {
    REQUIRE(true == rangeSet1.contains(r));
    REQUIRE(true == rangeSet2.contains(r));
  }
}

TEST_CASE("Range-IntersectionAndOverlaps") {
  static const int N = 100;
  std::vector<QBDI::Range<int>> testRanges;

  for (int i = 0; i < N; i++) {
    int start = rand() % 900;
    int end = start + rand() % 100 + 1;
    QBDI::Range<int> newRange{start, end};

    for (const QBDI::Range<int> &r : testRanges) {
      QBDI::RangeSet<int> set;
      set.add(newRange);
      set.add(r);

      // if the two range overlaps, the
      // size of set must be lesser than the sum of the size
      REQUIRE((set.size() < newRange.size() + r.size()) ==
              newRange.overlaps(r));
      REQUIRE(r.overlaps(newRange) == newRange.overlaps(r));
    }
    testRanges.push_back(newRange);
  }
}
