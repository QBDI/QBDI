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
#ifndef COMPAREDEXECUTOR_THUMB_H
#define COMPAREDEXECUTOR_THUMB_H

#include <sstream>
#include <string.h>
#include <string>

#include "TestSetup/InMemoryAssembler.h"
#include "TestSetup/ShellcodeTester.h"

#define CPU_CPU "cortex-a57"
#define CPU_MATTRS "neon", "thumb2", "v7"

extern const char *tGPRSave_s;
extern const char *tGPRShuffle_s;
extern const char *tRelativeAddressing_s;
extern const char *tFibonacciRecursion_s;
extern const char *tBranchCondTest_s;
extern const char *tBranchLinkCondTest_s;
extern const char *tBranchRegisterCondTest_s;
extern const char *tPushPopTest_s;
extern const char *tLdmiaStmdbWbackTest_s;
extern const char *tLdmdbStmiaWbackTest_s;
extern const char *tLdmiaStmdbTest_s;
extern const char *tLdmdbStmiaTest_s;
extern const char *tLdrPCTest_s;
extern const char *tLdrbPCTest_s;
extern const char *tLdrdPCTest_s;
extern const char *tLdrhPCTest_s;
extern const char *tLdrsbPCTest_s;
extern const char *tLdrshPCTest_s;
extern const char *tMovPCTest_s;
extern const char *tTBBTest_s;
extern const char *tTBHTest_s;
extern const char *tITCondTest_s;
extern const char *tldrexTest_s;
// extern const char *tConditionalBranching_s;
// extern const char *tStackTricks_s;
// extern const char *tSTLDMIA_s;
// extern const char *tSTLDMIB_s;
// extern const char *tSTLDMDA_s;
// extern const char *tSTLDMDB_s;
// extern const char *tSTMDB_LDMIA_post_s;
// extern const char *tSTMDA_LDMIB_post_s;
// extern const char *tSTMIB_LDMDA_post_s;
// extern const char *tSTMIA_LDMDB_post_s;

class ComparedExecutor_Thumb : public ShellcodeTester {

public:
  ComparedExecutor_Thumb() : ShellcodeTester(CPU_CPU, {CPU_MATTRS}) {}

  QBDI::Context jitExec(llvm::ArrayRef<uint8_t> code, QBDI::Context &inputCtx,
                        llvm::sys::MemoryBlock &stack);

  QBDI::Context realExec(llvm::ArrayRef<uint8_t> code, QBDI::Context &inputCtx,
                         llvm::sys::MemoryBlock &stack);

  InMemoryObject compileWithContextSwitch(const char *source);

  void initContext(QBDI::Context &ctx);
};

#endif
