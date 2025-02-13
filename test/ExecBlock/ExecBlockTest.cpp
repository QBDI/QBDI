/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2025 Quarkslab
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
#include <catch2/catch_test_macros.hpp>
#include <stdio.h>

#include "ExecBlockTest.h"
#include "PatchEmpty.h"

#include "ExecBlock/ExecBlock.h"
#include "Patch/ExecBlockPatch.h"
#include "Patch/Patch.h"
#include "Patch/PatchRule.h"
#include "Patch/RelocatableInst.h"

TEST_CASE_METHOD(ExecBlockTest, "ExecBlockTest-EmptyBasicBlock") {
  // Allocate ExecBlock
  QBDI::ExecBlock execBlock(*this, &this->vm);
  // Write an empty basic block
  QBDI::Patch::Vec empty;
  QBDI::SeqWriteResult res =
      execBlock.writeSequence(empty.begin(), empty.end());
  REQUIRE(res.seqID == QBDI::EXEC_BLOCK_FULL);
}

TEST_CASE_METHOD(ExecBlockTest, "ExecBlockTest-MultipleBasicBlock") {
  const QBDI::LLVMCPU &llvmcpu = this->getCPU(QBDI::CPUMode::DEFAULT);
  // Allocate ExecBlock
  QBDI::ExecBlock execBlock(*this, &this->vm);
  // Jit two different terminators
  QBDI::Patch::Vec terminator1;
  QBDI::Patch::Vec terminator2;
  terminator1.push_back(generateEmptyPatch(0x42424240, *this));
  terminator2.push_back(generateEmptyPatch(0x13371338, *this));
  terminator1[0].append(QBDI::getTerminator(llvmcpu, 0x42424240));
  terminator1[0].metadata.modifyPC = true;
  terminator2[0].append(QBDI::getTerminator(llvmcpu, 0x13371338));
  terminator2[0].metadata.modifyPC = true;
  QBDI::SeqWriteResult block1 =
      execBlock.writeSequence(terminator1.begin(), terminator1.end());
  QBDI::SeqWriteResult block2 =
      execBlock.writeSequence(terminator2.begin(), terminator2.end());
  // Are the seqID valid?
  REQUIRE(block2.seqID > block1.seqID);
  // Execute the two basic block and get the value of PC from the data block
  execBlock.selectSeq(block1.seqID);
  execBlock.execute();
  QBDI::rword pc1 =
      QBDI_GPR_GET(&execBlock.getContext()->gprState, QBDI::REG_PC);
  execBlock.selectSeq(block2.seqID);
  execBlock.execute();
  QBDI::rword pc2 =
      QBDI_GPR_GET(&execBlock.getContext()->gprState, QBDI::REG_PC);
  execBlock.selectSeq(block1.seqID);
  execBlock.execute();
  QBDI::rword pc3 =
      QBDI_GPR_GET(&execBlock.getContext()->gprState, QBDI::REG_PC);
  // block1 and block2 should have different PC values
  REQUIRE(pc1 != pc2);
  REQUIRE(pc1 == pc3);
}

TEST_CASE_METHOD(ExecBlockTest, "ExecBlockTest-BasicBlockOverload") {
  // Allocate ExecBlock
  QBDI::ExecBlock execBlock(*this, &this->vm);
  QBDI::Patch::Vec empty;
  QBDI::SeqWriteResult res;
  uint32_t i = 0;

  while ((res = execBlock.writeSequence(empty.begin(), empty.end())).seqID !=
         0xFFFF) {
    REQUIRE(res.seqID == i);
    REQUIRE(execBlock.getEpilogueOffset() >= (uint32_t)0);
    execBlock.selectSeq(res.seqID);
    execBlock.execute();
    i++;
  }
  INFO("Maximum basic block per exec block: " << i);
}
