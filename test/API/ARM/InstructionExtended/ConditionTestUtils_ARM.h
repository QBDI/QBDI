/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2026 Quarkslab
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
#ifndef QBDI_TEST_CONDITIONTESTUTILS_ARM_H
#define QBDI_TEST_CONDITIONTESTUTILS_ARM_H

#include <catch2/catch_test_macros.hpp>
#include "API/APITest.h"
#include "Patch/Utils.h"

#include "QBDI/InstAnalysis.h"
#include "QBDI/State.h"

namespace QBDITestBatch2 {

struct ConditionInfo {
  QBDI::ConditionType cond;
  const char *suffix;
};

constexpr ConditionInfo CONDITIONS[] = {
    {QBDI::CONDITION_EQUALS, "eq"},       {QBDI::CONDITION_NOT_EQUALS, "ne"},
    {QBDI::CONDITION_ABOVE_EQUALS, "hs"}, {QBDI::CONDITION_BELOW, "lo"},
    {QBDI::CONDITION_SIGN, "mi"},         {QBDI::CONDITION_NOT_SIGN, "pl"},
    {QBDI::CONDITION_OVERFLOW, "vs"},     {QBDI::CONDITION_NOT_OVERFLOW, "vc"},
    {QBDI::CONDITION_ABOVE, "hi"},        {QBDI::CONDITION_BELOW_EQUALS, "ls"},
    {QBDI::CONDITION_GREAT_EQUALS, "ge"}, {QBDI::CONDITION_LESS, "lt"},
    {QBDI::CONDITION_GREAT, "gt"},        {QBDI::CONDITION_LESS_EQUALS, "le"},
};

constexpr size_t CONDITIONS_COUNT = sizeof(CONDITIONS) / sizeof(CONDITIONS[0]);

[[maybe_unused]] inline const ConditionInfo &conditionForIndex(size_t i) {
  unsigned int seed = seed_random();
  return CONDITIONS[(i + seed) % CONDITIONS_COUNT];
}

constexpr QBDI::rword APSR_N = (1u << 31);
constexpr QBDI::rword APSR_Z = (1u << 30);
constexpr QBDI::rword APSR_C = (1u << 29);
constexpr QBDI::rword APSR_V = (1u << 28);
constexpr QBDI::rword APSR_NZCV_MASK = APSR_N | APSR_Z | APSR_C | APSR_V;

[[maybe_unused]] inline QBDI::rword nzcvForCondition(QBDI::ConditionType cond,
                                                     bool taken) {
  switch (cond) {
    case QBDI::CONDITION_EQUALS:
      return taken ? APSR_Z : 0;
    case QBDI::CONDITION_NOT_EQUALS:
      return taken ? 0 : APSR_Z;
    case QBDI::CONDITION_ABOVE_EQUALS:
      return taken ? APSR_C : 0;
    case QBDI::CONDITION_BELOW:
      return taken ? 0 : APSR_C;
    case QBDI::CONDITION_SIGN:
      return taken ? APSR_N : 0;
    case QBDI::CONDITION_NOT_SIGN:
      return taken ? 0 : APSR_N;
    case QBDI::CONDITION_OVERFLOW:
      return taken ? APSR_V : 0;
    case QBDI::CONDITION_NOT_OVERFLOW:
      return taken ? 0 : APSR_V;
    case QBDI::CONDITION_ABOVE:
      return taken ? APSR_C : 0;
    case QBDI::CONDITION_BELOW_EQUALS:
      return taken ? 0 : APSR_C;
    case QBDI::CONDITION_GREAT_EQUALS:
      return taken ? 0 : APSR_N;
    case QBDI::CONDITION_LESS:
      return taken ? APSR_N : 0;
    case QBDI::CONDITION_GREAT:
      return taken ? 0 : APSR_Z;
    case QBDI::CONDITION_LESS_EQUALS:
      return taken ? APSR_Z : 0;
    default:
      REQUIRE(false);
      return 0;
  }
}

[[maybe_unused]] inline void
setConditionCPSR(APITest &fixture, QBDI::ConditionType cond, bool taken) {
  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->cpsr = (state->cpsr & ~APSR_NZCV_MASK) | nzcvForCondition(cond, taken);
  fixture.vm.setGPRState(state);
}

} // namespace QBDITestBatch2

#endif // QBDI_TEST_CONDITIONTESTUTILS_ARM_H
