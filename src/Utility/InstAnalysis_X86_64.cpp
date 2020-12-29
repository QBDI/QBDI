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

#include <map>
#include <string>

#include "MCTargetDesc/X86BaseInfo.h"

#include "InstAnalysis_prive.h"

namespace QBDI {

unsigned MCOI_COND_CODE = llvm::X86::OperandType::OPERAND_COND_CODE;

static const std::map<unsigned, std::string> CondNameMap = {
    { llvm::X86::CondCode::COND_O, "O"},
    { llvm::X86::CondCode::COND_NO, "NO"},
    { llvm::X86::CondCode::COND_B, "B"},
    { llvm::X86::CondCode::COND_AE, "AE"},
    { llvm::X86::CondCode::COND_E, "E"},
    { llvm::X86::CondCode::COND_NE, "NE"},
    { llvm::X86::CondCode::COND_BE, "BE"},
    { llvm::X86::CondCode::COND_A, "A"},
    { llvm::X86::CondCode::COND_S, "S"},
    { llvm::X86::CondCode::COND_NS, "NS"},
    { llvm::X86::CondCode::COND_P, "P"},
    { llvm::X86::CondCode::COND_NP, "NP"},
    { llvm::X86::CondCode::COND_L, "L"},
    { llvm::X86::CondCode::COND_GE, "GE"},
    { llvm::X86::CondCode::COND_LE, "LE"},
    { llvm::X86::CondCode::COND_G, "G"},
    { llvm::X86::CondCode::COND_NE_OR_P, "NE_OR_P"},
    { llvm::X86::CondCode::COND_E_AND_NP, "E_AND_NP"},
};


const char* getName_MCOI_COND(unsigned value) {
    auto r = CondNameMap.find(value);
    if (r == CondNameMap.end())
        return nullptr;
    else
        return r->second.c_str();
}

}
