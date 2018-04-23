/*
 * This file is part of QBDI.
 *
 * Copyright 2018 Quarkslab
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

#ifndef LLVMCPU_H
#define LLVMCPU_H

#include <memory>

#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCObjectFileInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"

namespace QBDI {

class LLVMCPU {

private:

    std::string                              tripleName;
    std::string                              cpu;
    std::string                              arch;
    std::vector<std::string>                 mattrs;
    const llvm::Target*                      target;

    std::unique_ptr<llvm::MCAsmBackend>      MAB;
    std::unique_ptr<llvm::MCAsmInfo>         MAI;
    std::unique_ptr<llvm::MCCodeEmitter>     MCE;
    std::unique_ptr<llvm::MCContext>         MCTX;
    std::unique_ptr<llvm::MCInstrInfo>       MII;
    std::unique_ptr<llvm::MCObjectFileInfo>  MOFI;
    std::unique_ptr<llvm::MCRegisterInfo>    MRI;
    std::unique_ptr<llvm::MCSubtargetInfo>   MSTI;

public:

    LLVMCPU(const std::string& cpu = "", const std::string& arch = "", const std::vector<std::string>& mattrs = {});

    const llvm::Target*        getTarget() { return target;     }

    llvm::MCAsmBackend*        getMAB()    { return MAB.get();  }

    llvm::MCAsmInfo*           getMAI()    { return MAI.get();  }

    llvm::MCContext*           getMCTX()   { return MCTX.get(); }

    llvm::MCCodeEmitter*       getMCE()    { return MCE.get();  }

    llvm::MCInstrInfo*         getMII()    { return MII.get();  }

    llvm::MCObjectFileInfo*    getMOFI()   { return MOFI.get(); }
    
    llvm::MCRegisterInfo*      getMRI()    { return MRI.get();  } 
    
    llvm::MCSubtargetInfo*     getMSTI()   { return MSTI.get(); }
};

}

#endif // LLVMCPU_H
