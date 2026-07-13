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
#include "Utils/AArch64BaseInfo.h"
#include "llvm/MC/MCInst.h"

#include "Patch/AARCH64/InstTransform_AARCH64.h"
#include "Patch/Patch.h"
#include "Patch/TempManager.h"
#include "Utility/LogSys.h"

namespace QBDI {

void SetOperandW::transform(llvm::MCInst &inst, rword address, size_t instSize,
                            TempManager &temp_manager) const {
  QBDI_REQUIRE_ABORT(opn < inst.getNumOperands(), "Invalid operand {} {}", opn,
                     temp_manager.getPatch());
  Reg tempReg = temp_manager.getRegForTemp(temp);
  RegLLVM wreg = llvm::getWRegFromXReg(tempReg.getValue());
  inst.getOperand(opn).setReg(wreg.getValue());
}

} // namespace QBDI
