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
#include <memory>

#include "ExecBlockManagerTest.h"
#include "PatchEmpty.h"

#include "QBDI/State.h"
#include "ExecBlock/Context.h"
#include "ExecBlock/ExecBlock.h"
#include "ExecBlock/ExecBlockManager.h"
#include "Patch/ExecBlockPatch.h"
#include "Patch/InstMetadata.h"
#include "Patch/Patch.h"
#include "Patch/RelocatableInst.h"

QBDI::Patch::Vec getEmptyBB(QBDI::rword address,
                            const QBDI::LLVMCPUs &llvmcpu) {
  QBDI::Patch::Vec bb;
  bb.push_back(generateEmptyPatch(address, llvmcpu));
  return bb;
}

TEST_CASE_METHOD(ExecBlockManagerTest,
                 "ExecBlockManagerTest-BasicBlockLookup") {
  QBDI::ExecBlockManager execBlockManager(*this, &this->vm);

  execBlockManager.writeBasicBlock(getEmptyBB(0x42424240, *this), 1);
  REQUIRE(nullptr == execBlockManager.getProgrammedExecBlock(
                         0x13371338, QBDI::CPUMode::DEFAULT));
  REQUIRE(nullptr != execBlockManager.getProgrammedExecBlock(
                         0x42424240, QBDI::CPUMode::DEFAULT));
}

TEST_CASE_METHOD(ExecBlockManagerTest, "ExecBlockManagerTest-ClearCache") {
  QBDI::ExecBlockManager execBlockManager(*this, &this->vm);

  execBlockManager.writeBasicBlock(getEmptyBB(0x42424240, *this), 1);
  REQUIRE(nullptr != execBlockManager.getProgrammedExecBlock(
                         0x42424240, QBDI::CPUMode::DEFAULT));
  execBlockManager.clearCache();
  REQUIRE(nullptr == execBlockManager.getProgrammedExecBlock(
                         0x42424240, QBDI::CPUMode::DEFAULT));
}

TEST_CASE_METHOD(ExecBlockManagerTest, "ExecBlockManagerTest-ExecBlockReuse") {
  QBDI::ExecBlockManager execBlockManager(*this, &this->vm);

  execBlockManager.writeBasicBlock(getEmptyBB(0x42424240, *this), 1);
  execBlockManager.writeBasicBlock(getEmptyBB(0x42424244, *this), 1);
  REQUIRE(execBlockManager.getProgrammedExecBlock(0x42424240,
                                                  QBDI::CPUMode::DEFAULT) ==
          execBlockManager.getProgrammedExecBlock(0x42424244,
                                                  QBDI::CPUMode::DEFAULT));
}

TEST_CASE_METHOD(ExecBlockManagerTest,
                 "ExecBlockManagerTest-ExecBlockRegions") {
  QBDI::ExecBlockManager execBlockManager(*this, &this->vm);

  execBlockManager.writeBasicBlock(getEmptyBB(0x42424240, *this), 1);
  execBlockManager.writeBasicBlock(getEmptyBB(0x24242424, *this), 1);
  REQUIRE(execBlockManager.getProgrammedExecBlock(0x42424240,
                                                  QBDI::CPUMode::DEFAULT) !=
          execBlockManager.getProgrammedExecBlock(0x24242424,
                                                  QBDI::CPUMode::DEFAULT));
}

TEST_CASE_METHOD(ExecBlockManagerTest, "ExecBlockManagerTest-ExecBlockAlloc") {
  QBDI::ExecBlockManager execBlockManager(*this, &this->vm);
  QBDI::rword address = 0;

  for (address = 0; address < 0x1000; address++) {
    execBlockManager.writeBasicBlock(getEmptyBB(address, *this), 1);
  }

  REQUIRE(
      execBlockManager.getProgrammedExecBlock(0, QBDI::CPUMode::DEFAULT) !=
      execBlockManager.getProgrammedExecBlock(0xfff, QBDI::CPUMode::DEFAULT));
}

TEST_CASE_METHOD(ExecBlockManagerTest, "ExecBlockManagerTest-CacheRewrite") {
  QBDI::ExecBlockManager execBlockManager(*this, &this->vm);
  unsigned int i = 0;

  execBlockManager.writeBasicBlock(getEmptyBB(0x42424240, *this), 1);
  QBDI::ExecBlock *block1 = execBlockManager.getProgrammedExecBlock(
      0x42424240, QBDI::CPUMode::DEFAULT);
  for (i = 0; i < 0x1000; i++) {
    execBlockManager.writeBasicBlock(getEmptyBB(0x42424240, *this), 1);
  }
  QBDI::ExecBlock *block2 = execBlockManager.getProgrammedExecBlock(
      0x42424240, QBDI::CPUMode::DEFAULT);

  REQUIRE(block1 == block2);
}

TEST_CASE_METHOD(ExecBlockManagerTest,
                 "ExecBlockManagerTest-MultipleBasicBlockExecution") {
  const QBDI::LLVMCPU &llvmcpu = this->getCPU(QBDI::CPUMode::DEFAULT);
  QBDI::ExecBlockManager execBlockManager(*this, &this->vm);
  QBDI::ExecBlock *block = nullptr;
  // Jit two different terminators
  QBDI::Patch::Vec terminator1 = getEmptyBB(0x42424240, *this);
  QBDI::Patch::Vec terminator2 = getEmptyBB(0x13371338, *this);
  terminator1[0].append(QBDI::getTerminator(llvmcpu, 0x42424240));
  terminator1[0].metadata.modifyPC = true;
  terminator2[0].append(QBDI::getTerminator(llvmcpu, 0x13371338));
  terminator2[0].metadata.modifyPC = true;
  execBlockManager.writeBasicBlock(std::move(terminator1), 1);
  execBlockManager.writeBasicBlock(std::move(terminator2), 1);
  // Execute the two basic block and get the value of PC from the data block
  block = execBlockManager.getProgrammedExecBlock(0x42424240,
                                                  QBDI::CPUMode::DEFAULT);
  REQUIRE(nullptr != block);
  block->execute();
  REQUIRE((QBDI::rword)0x42424240 ==
          QBDI_GPR_GET(&block->getContext()->gprState, QBDI::REG_PC));

  block = execBlockManager.getProgrammedExecBlock(0x13371338,
                                                  QBDI::CPUMode::DEFAULT);
  REQUIRE(nullptr != block);
  block->execute();
  REQUIRE((QBDI::rword)0x13371338 ==
          QBDI_GPR_GET(&block->getContext()->gprState, QBDI::REG_PC));
}

TEST_CASE_METHOD(ExecBlockManagerTest, "ExecBlockManagerTest-Stresstest") {
  const unsigned align = 4;
  const QBDI::LLVMCPU &llvmcpu = this->getCPU(QBDI::CPUMode::DEFAULT);
  QBDI::ExecBlockManager execBlockManager(*this, &this->vm);
  QBDI::rword address = 0;

  for (address = 0; address < 1000 * align; address += align) {
    QBDI::Patch::Vec terminator = getEmptyBB(address, *this);
    terminator[0].append(QBDI::getTerminator(llvmcpu, address));
    terminator[0].metadata.modifyPC = true;
    execBlockManager.writeBasicBlock(std::move(terminator), 1);
    QBDI::ExecBlock *block = execBlockManager.getProgrammedExecBlock(
        address, QBDI::CPUMode::DEFAULT);
    REQUIRE(nullptr != block);
    block->execute();
    REQUIRE(address ==
            QBDI_GPR_GET(&block->getContext()->gprState, QBDI::REG_PC));
  }
  for (; address > 0; address -= align) {
    QBDI::ExecBlock *block = execBlockManager.getProgrammedExecBlock(
        address - align, QBDI::CPUMode::DEFAULT);
    REQUIRE(nullptr != block);
    block->execute();
    REQUIRE(address - align ==
            QBDI_GPR_GET(&block->getContext()->gprState, QBDI::REG_PC));
  }
}
