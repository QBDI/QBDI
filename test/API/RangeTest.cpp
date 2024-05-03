/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2024 Quarkslab
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
#include "Patch/Utils.h"

template <typename T>
void checkRangeSetInvariant(const QBDI::RangeSet<T> &set) {
  const std::vector<QBDI::Range<T>> &ranges = set.getRanges();

  for (const auto &r : ranges) {
    // no empty ranges in the set
    REQUIRE(r.start() < r.end());
  }

  for (unsigned i = 1; i < ranges.size(); i++) {
    // the range in the set are sorted.
    // Moreever, the set should be minimized
    REQUIRE(ranges[i - 1].end() < ranges[i].start());
  }
}

TEST_CASE("Range-UnitTest") {

  std::vector<QBDI::Range<int>> testRanges = {
      {50, 60}, {85, 90}, {10, 20}, {25, 30}, {65, 70}, {70, 75},  {74, 80},
      {80, 84}, {60, 85}, {40, 60}, {9, 120}, {55, 67}, {80, 200}, {5, 150},
  };
  QBDI::RangeSet<int> rangeSet;
  QBDI::RangeSet<int> rangeSetInv;
  rangeSetInv.add({0, 1000});

  for (unsigned i = 0; i < testRanges.size(); i++) {
    rangeSet.add(testRanges[i]);
    rangeSetInv.remove(testRanges[i]);

    checkRangeSetInvariant(rangeSet);
    checkRangeSetInvariant(rangeSetInv);

    for (unsigned j = 0; j <= i; j++) {
      REQUIRE(rangeSet.contains(testRanges[i]));
      REQUIRE_FALSE(rangeSetInv.contains(testRanges[i]));
      REQUIRE_FALSE(rangeSetInv.overlaps(testRanges[i]));
    }

    for (const auto &r : rangeSet.getRanges()) {
      REQUIRE_FALSE(rangeSetInv.contains(r));
      REQUIRE_FALSE(rangeSetInv.overlaps(r));
    }

    for (const auto &r : rangeSetInv.getRanges()) {
      REQUIRE_FALSE(rangeSet.contains(r));
      REQUIRE_FALSE(rangeSet.overlaps(r));
    }
  }
}

TEST_CASE("Range-StateIntegrity") {
  INFO("TEST_SEED=" << seed_random());
  static const unsigned N = 100;
  std::vector<QBDI::Range<int>> testRanges;
  QBDI::RangeSet<int> rangeSet;

  for (unsigned i = 0; i < N; i++) {
    int start = get_random() % 900;
    int end = start + get_random() % 100 + 1;
    int delta = -rangeSet.size();
    QBDI::Range<int> r(start, end);

    testRanges.push_back(r);
    rangeSet.add(r);
    delta += rangeSet.size();

    REQUIRE(r.size() >= delta);
    REQUIRE(rangeSet.contains(r));
    REQUIRE(rangeSet.contains(r.start()));
    REQUIRE(rangeSet.contains(r.end() - 1));
    checkRangeSetInvariant(rangeSet);
  }

  for (unsigned i = 0; i < N; i++) {
    int delta = rangeSet.size();
    QBDI::Range<int> r = testRanges.back();

    testRanges.pop_back();
    rangeSet.remove(r);
    delta -= rangeSet.size();

    REQUIRE(r.size() >= delta);
    REQUIRE_FALSE(rangeSet.contains(r));
    REQUIRE_FALSE(rangeSet.contains(r.start()));
    REQUIRE_FALSE(rangeSet.contains(r.end() - 1));
    checkRangeSetInvariant(rangeSet);
  }
  REQUIRE(0 == rangeSet.size());
}

template <typename T>
void randomPermutation(std::vector<T> &v) {
  size_t i = 0;
  for (i = 0; i < v.size(); i++) {
    size_t p = get_random() % v.size();
    T t = v[i];
    v[i] = v[p];
    v[p] = t;
  }
}

TEST_CASE("Range-Commutativity") {
  INFO("TEST_SEED=" << seed_random());
  static const unsigned N = 100;
  std::vector<QBDI::Range<int>> testRanges;
  QBDI::RangeSet<int> rangeSet;

  for (unsigned i = 0; i < N; i++) {
    int start = get_random() % 1000000;
    int end = start + 1 + get_random() % 10000;
    QBDI::Range<int> r(start, end);
    testRanges.push_back(r);
    rangeSet.add(r);
    checkRangeSetInvariant(rangeSet);
  }

  for (unsigned c = 0; c < N; c++) {
    QBDI::RangeSet<int> permutedRangeSet;

    randomPermutation(testRanges);
    for (unsigned i = 0; i < N; i++) {
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
  INFO("TEST_SEED=" << seed_random());
  static const unsigned N = 100;
  QBDI::RangeSet<int> rangeSet1;
  QBDI::RangeSet<int> rangeSet2;
  QBDI::RangeSet<int> intersection1;
  QBDI::RangeSet<int> intersection2;

  for (unsigned i = 0; i < N; i++) {
    int start = get_random() % 1000000;
    int end = start + 1 + get_random() % 10000;
    rangeSet1.add(QBDI::Range<int>(start, end));
    checkRangeSetInvariant(rangeSet1);
  }

  for (unsigned i = 0; i < N; i++) {
    int start = get_random() % 1000000;
    int end = start + 1 + get_random() % 10000;
    rangeSet2.add(QBDI::Range<int>(start, end));
    checkRangeSetInvariant(rangeSet2);
  }

  intersection1.add(rangeSet1);
  intersection1.intersect(rangeSet2);
  intersection2.add(rangeSet2);
  intersection2.intersect(rangeSet1);

  REQUIRE(intersection1 == intersection2);
  checkRangeSetInvariant(intersection1);
  checkRangeSetInvariant(intersection2);

  for (QBDI::Range<int> r : intersection1.getRanges()) {
    REQUIRE(true == rangeSet1.contains(r));
    REQUIRE(true == rangeSet2.contains(r));
  }
}

TEST_CASE("Range-IntersectionAndOverlaps") {
  INFO("TEST_SEED=" << seed_random());
  static const unsigned N = 100;
  std::vector<QBDI::Range<int>> testRanges;

  for (unsigned i = 0; i < N; i++) {
    int start = get_random() % 900;
    int end = start + get_random() % 100 + 1;
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
      checkRangeSetInvariant(set);
    }
    testRanges.push_back(newRange);
  }
}
