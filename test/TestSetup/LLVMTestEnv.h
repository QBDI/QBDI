/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2021 Quarkslab
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
#ifndef LLVMTESTENV_H
#define LLVMTESTENV_H

#include <memory>
#include <string>
#include <vector>

#include "Options.h"

namespace llvm {
  class MCAsmInfo;
  class MCCodeEmitter;
  class MCContext;
  class MCInstrInfo;
  class MCObjectFileInfo;
  class MCRegisterInfo;
  class MCSubtargetInfo;
  class Target;
}

namespace QBDI {
  class Assembly;
}

class LLVMTestEnv {
protected:

    std::unique_ptr<llvm::MCAsmInfo>         MAI;
    std::unique_ptr<llvm::MCCodeEmitter>     MCE;
    std::unique_ptr<llvm::MCContext>         MCTX;
    std::unique_ptr<llvm::MCInstrInfo>       MCII;
    std::unique_ptr<llvm::MCObjectFileInfo>  MOFI;
    std::unique_ptr<llvm::MCRegisterInfo>    MRI;
    std::unique_ptr<llvm::MCSubtargetInfo>   MSTI;
    std::unique_ptr<QBDI::Assembly>          assembly;
    const llvm::Target*                      processTarget;
    std::string                              tripleName;
    std::string                              cpu;
    std::vector<std::string>                 mattrs;

public:

    LLVMTestEnv(std::string cpu = "", std::vector<std::string> mattrs = {}, QBDI::Options opts = QBDI::Options::NO_OPT);
    ~LLVMTestEnv();
};

#endif
