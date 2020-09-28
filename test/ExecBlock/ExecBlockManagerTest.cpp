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
#include <gtest/gtest.h>

#include "ExecBlockManagerTest.h"

#include "ExecBlock/ExecBlockManager.h"
#include "Patch/Patch.h"
#include "Patch/PatchRule.h"
#include "Patch/PatchRules.h"
#include "Utility/Assembly.h"


QBDI::Patch::Vec getEmptyBB(QBDI::rword address) {
    QBDI::Patch::Vec bb;
    bb.push_back(QBDI::Patch());
    bb[0].metadata.address = address;
    bb[0].metadata.instSize = 1;
    return bb;
}

TEST_F(ExecBlockManagerTest, BasicBlockLookup) {
    QBDI::ExecBlockManager execBlockManager(*MCII, *MRI, *assembly);

    execBlockManager.writeBasicBlock(getEmptyBB(0x42424242));
    ASSERT_EQ(nullptr, execBlockManager.getProgrammedExecBlock(0x13371337));
    ASSERT_NE(nullptr, execBlockManager.getProgrammedExecBlock(0x42424242));
}

TEST_F(ExecBlockManagerTest, ClearCache) {
    QBDI::ExecBlockManager execBlockManager(*MCII, *MRI, *assembly);

    execBlockManager.writeBasicBlock(getEmptyBB(0x42424242));
    ASSERT_NE(nullptr, execBlockManager.getProgrammedExecBlock(0x42424242));
    execBlockManager.clearCache();
    ASSERT_EQ(nullptr, execBlockManager.getProgrammedExecBlock(0x42424242));
}

TEST_F(ExecBlockManagerTest, ExecBlockReuse) {
    QBDI::ExecBlockManager execBlockManager(*MCII, *MRI, *assembly);

    execBlockManager.writeBasicBlock(getEmptyBB(0x42424242));
    execBlockManager.writeBasicBlock(getEmptyBB(0x42424243));
    ASSERT_EQ(execBlockManager.getProgrammedExecBlock(0x42424242),
              execBlockManager.getProgrammedExecBlock(0x42424243));
}

TEST_F(ExecBlockManagerTest, ExecBlockRegions) {
    QBDI::ExecBlockManager execBlockManager(*MCII, *MRI, *assembly);

    execBlockManager.writeBasicBlock(getEmptyBB(0x42424242));
    execBlockManager.writeBasicBlock(getEmptyBB(0x24242424));
    ASSERT_NE(execBlockManager.getProgrammedExecBlock(0x42424242),
              execBlockManager.getProgrammedExecBlock(0x24242424));
}

TEST_F(ExecBlockManagerTest, ExecBlockAlloc) {
    QBDI::ExecBlockManager execBlockManager(*MCII, *MRI, *assembly);
    QBDI::rword address = 0;

    for(address = 0; address < 0x1000; address++) {
        execBlockManager.writeBasicBlock(getEmptyBB(address));
    }

    ASSERT_NE(execBlockManager.getProgrammedExecBlock(0),
              execBlockManager.getProgrammedExecBlock(0xfff));
}

TEST_F(ExecBlockManagerTest, CacheRewrite) {
    QBDI::ExecBlockManager execBlockManager(*MCII, *MRI, *assembly);
    unsigned int i = 0;

    execBlockManager.writeBasicBlock(getEmptyBB(0x42424242));
    QBDI::ExecBlock* block1 = execBlockManager.getProgrammedExecBlock(0x42424242);
    for(i = 0; i < 0x1000; i++) {
        execBlockManager.writeBasicBlock(getEmptyBB(0x42424242));
    }
    QBDI::ExecBlock* block2 = execBlockManager.getProgrammedExecBlock(0x42424242);

    ASSERT_EQ(block1, block2);
}

TEST_F(ExecBlockManagerTest, MultipleBasicBlockExecution) {
    QBDI::ExecBlockManager execBlockManager(*MCII, *MRI, *assembly);
    QBDI::ExecBlock *block = nullptr;
    // Jit two different terminators
    QBDI::Patch::Vec terminator1 = getEmptyBB(0x42424242);
    QBDI::Patch::Vec terminator2 = getEmptyBB(0x13371337);
    terminator1[0].append(QBDI::getTerminator(0x42424242));
    terminator2[0].append(QBDI::getTerminator(0x13371337));
    execBlockManager.writeBasicBlock(terminator1);
    execBlockManager.writeBasicBlock(terminator2);
    // Execute the two basic block and get the value of PC from the data block
    block = execBlockManager.getProgrammedExecBlock(0x42424242);
    ASSERT_NE(nullptr, block);
    block->execute();
    ASSERT_EQ((QBDI::rword) 0x42424242, QBDI_GPR_GET(&block->getContext()->gprState, QBDI::REG_PC));

    block = execBlockManager.getProgrammedExecBlock(0x13371337);
    ASSERT_NE(nullptr, block);
    block->execute();
    ASSERT_EQ((QBDI::rword) 0x13371337, QBDI_GPR_GET(&block->getContext()->gprState, QBDI::REG_PC));
}

TEST_F(ExecBlockManagerTest, Stresstest) {
    QBDI::ExecBlockManager execBlockManager(*MCII, *MRI, *assembly);
    QBDI::rword address = 0;

    for(address = 0; address < 1000; address++) {
        QBDI::Patch::Vec terminator = getEmptyBB(address);
        terminator[0].append(QBDI::getTerminator(address));
        execBlockManager.writeBasicBlock(terminator);
        QBDI::ExecBlock *block = execBlockManager.getProgrammedExecBlock(address);
        ASSERT_NE(nullptr, block);
        block->execute();
        ASSERT_EQ(address, QBDI_GPR_GET(&block->getContext()->gprState, QBDI::REG_PC));
    }
    for(; address > 0; address--) {
        QBDI::ExecBlock *block = execBlockManager.getProgrammedExecBlock(address-1);
        ASSERT_NE(nullptr, block);
        block->execute();
        ASSERT_EQ(address - 1, QBDI_GPR_GET(&block->getContext()->gprState, QBDI::REG_PC));
    }
}
