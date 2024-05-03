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
#ifndef PATCHRULEASSEMBLYBASE_H
#define PATCHRULEASSEMBLYBASE_H

#include <memory>
#include <vector>

#include "QBDI/Options.h"
#include "QBDI/State.h"

namespace llvm {
class MCInst;
} // namespace llvm

namespace QBDI {
class LLVMCPU;
class Patch;

/*! The patchRule allows QBDI to apply property for an instruction to the next
 *  one (in the same basicblock).
 */
class PatchRuleAssemblyBase {
public:
  /*! Change QBDI options associated with the VM
   *
   * @param[in] opts  The new options
   *
   * @return  True if the whole cache should be invalidated
   */
  virtual bool changeOptions(Options opts) = 0;

  /*! Generate a patch for this MCInst
   *
   * @param[in] inst      The instruction to instrument
   * @param[in] address   The address of the instruction
   * @param[in] instSize  The size of the instruction
   * @param[in] llvmcpu   The CPU used for this instruction. The same CPU is
   *                      used for all instruction in the same BB
   * @param[in] patchList The list of patch generated
   *
   * @return  True if this instruction is the last of a basicBlock
   */
  virtual bool generate(const llvm::MCInst &inst, rword address,
                        uint32_t instSize, const LLVMCPU &llvmcpu,
                        std::vector<Patch> &patchList) = 0;

  /*! Clean patchList if the basicBlock should be end early (ie: if error in the
   *  in the next instruction or read unmapped memory). This flush the pending
   *  patch and remove unfinish Patch from patchList
   *
   * @param[in] llvmcpu   The CPU used for this instruction. The same CPU is
   *                      used for all instruction in the same BB
   * @param[in] patchList The list of patch generated
   *
   * @return  True if the operation success. Otherwise, patchList should not be
   *          used
   */
  virtual bool earlyEnd(const LLVMCPU &llvmcpu,
                        std::vector<Patch> &patchList) = 0;
};

} // namespace QBDI

#endif // PATCHRULEASSEMBLYBASE_H
