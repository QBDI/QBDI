/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2022 Quarkslab
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
#include "Engine/LLVMCPU.h"
#include "ExecBlock/ExecBlock.h"
#include "ExecBroker/ExecBroker.h"
#include "Patch/AARCH64/Layer2_AARCH64.h"
#include "Patch/AARCH64/PatchGenerator_AARCH64.h"
#include "Patch/AARCH64/RelocatableInst_AARCH64.h"
#include "Patch/Patch.h"
#include "Utility/LogSys.h"

#include "llvm/MC/MCInst.h"

namespace QBDI {
static const size_t SCAN_DISTANCE = 2;

void ExecBroker::initExecBrokerSequences(const LLVMCPUs &llvmCPUs) {
  llvm::MCInst nopInst = nop();
  const LLVMCPU &llvmCPU = llvmCPUs.getCPU(CPUMode::DEFAULT);

  // Sequence Broker with LR
  // =======================
  {
    QBDI_DEBUG("Create Sequence Broker LR");
    QBDI::Patch::Vec brokerLR;
    brokerLR.push_back(QBDI::Patch(nopInst, 0x4, 4, llvmCPU));
    Patch &pLR = brokerLR[0];
    pLR.finalize = true;

    // target BTIj
    pLR.append(RelocTag::unique(RelocTagPatchBegin));
    pLR.append(GenBTI().genReloc(llvmCPU));

    // Prepare to Jump
    // 1. Store jumpAddress in LR
    pLR.append(
        Ldr(Reg(REG_LR), Offset(offsetof(Context, hostState.brokerAddr))));

    // 2. backup TPIDR and restore X28 and SR
    pLR.append(FullRegisterRestore(true).genReloc(llvmCPU));

    // 3. Jump to the target. Use BLR to change LR to the hook return address
    pLR.append(Blr(Reg(REG_LR)));

    // use RelocTagPatchInstEnd to mark the return address
    pLR.append(RelocTag::unique(RelocTagPatchInstBegin));
    pLR.append(RelocTag::unique(RelocTagPatchInstEnd));

    // Hook
    // Backup X28 and SR and reset SR to base addr
    pLR.append(FullRegisterReset(true).genReloc(llvmCPU));

    // This sequence doesn't need a terminator
    pLR.setModifyPC(true);

    // Write the Patch in the ExecBlock
    SeqWriteResult res =
        transferBlock->writeSequence(brokerLR.begin(), brokerLR.end());

    QBDI_REQUIRE_ABORT(res.patchWritten == 1, "Fail to write Sequence Broker");

    uint16_t instID = transferBlock->getSeqStart(res.seqID);

    std::vector<TagInfo> retAddr =
        transferBlock->queryTagByInst(instID, RelocTagPatchInstEnd);
    QBDI_REQUIRE(retAddr.size() == 1);

    archData.transfertLR.seqID = res.seqID;
    archData.transfertLR.hook = transferBlock->getAddressTag(retAddr[0]);
    QBDI_DEBUG("Sequence Broker LR: id={} hook=0x{:x}",
               archData.transfertLR.seqID, archData.transfertLR.hook);
  }

  // Sequence Broker with X28
  // ========================
  {
    QBDI_DEBUG("Create Sequence Broker X28");
    QBDI::Patch::Vec brokerX28;
    brokerX28.push_back(QBDI::Patch(nopInst, 0x4, 4, llvmCPU));
    Patch &pX28 = brokerX28[0];
    pX28.finalize = true;

    // target BTIj
    pX28.append(RelocTag::unique(RelocTagPatchBegin));
    pX28.append(GenBTI().genReloc(llvmCPU));

    // Prepare to Jump
    // 1. Store jumpAddress in X28
    pX28.append(Ldr(Reg(28), Offset(offsetof(Context, hostState.brokerAddr))));

    // 2. Restore ScratchRegister
    pX28.append(FullRegisterRestore(false).genReloc(llvmCPU));

    // 3. Jump to the target using X28.
    // Don't link as LR does'nt have a return address
    pX28.append(Br(Reg(28)));

    // use RelocTagPatchInstEnd to mark the return address
    pX28.append(RelocTag::unique(RelocTagPatchInstBegin));
    pX28.append(RelocTag::unique(RelocTagPatchInstEnd));

    // Hook
    // 1. Backup SR and reset it to base address
    pX28.append(FullRegisterReset(false).genReloc(llvmCPU));

    // This sequence doesn't need a terminator
    pX28.setModifyPC(true);

    // Write the Patch in the ExecBlock
    SeqWriteResult res =
        transferBlock->writeSequence(brokerX28.begin(), brokerX28.end());
    QBDI_REQUIRE_ABORT(res.patchWritten == 1, "Fail to write Sequence Broker");

    uint16_t instID = transferBlock->getSeqStart(res.seqID);

    std::vector<TagInfo> retAddr =
        transferBlock->queryTagByInst(instID, RelocTagPatchInstEnd);
    QBDI_REQUIRE(retAddr.size() == 1);

    archData.transfertX28.seqID = res.seqID;
    archData.transfertX28.hook = transferBlock->getAddressTag(retAddr[0]);

    QBDI_DEBUG("Sequence Broker X28: id={} hook=0x{:x}",
               archData.transfertX28.seqID, archData.transfertX28.hook);
  }
}

rword *ExecBroker::getReturnPoint(GPRState *gprState) const {
  auto ptr = reinterpret_cast<rword *>(gprState->sp);

  if (isInstrumented(gprState->lr)) {
    QBDI_DEBUG("Found instrumented return address in LR register");
    return &(gprState->lr);
  }

  for (size_t i = 0; i < SCAN_DISTANCE; ++i) {
    if (isInstrumented(ptr[i])) {
      QBDI_DEBUG("Found instrumented return address on the stack at {:p}",
                 reinterpret_cast<void *>(&(ptr[i])));
      return &(ptr[i]);
    }
  }

  QBDI_DEBUG("LR register does not contain an instrumented return address");
  return nullptr;
}

bool ExecBroker::transferExecution(rword addr, GPRState *gprState,
                                   FPRState *fprState) {

  // Search all return address
  rword *ptr = getReturnPoint(gprState);
  rword *ptr2 = nullptr;
  rword returnAddress = *ptr;

  if constexpr (is_linux) {

    /* On Linux, the resolution of a plt symbol may use the follow code:
     *
     * 0000000000000820 <.plt>:
     *  820:   a9bf7bf0        stp     x16, x30, [sp, #-16]!
     *  824:   90000090        adrp    x16, 10000 <__FRAME_END__+0xf430>
     *  828:   f947fe11        ldr     x17, [x16, #4088]
     *  82c:   913fe210        add     x16, x16, #0xff8
     *  830:   d61f0220        br      x17
     *  834:   d503201f        nop
     *  838:   d503201f        nop
     *  83c:   d503201f        nop
     *
     * 0000000000000840 <__cxa_finalize@plt>:
     *  840:   b0000090        adrp    x16, 11000 <__cxa_finalize@GLIBC_2.17>
     *  844:   f9400211        ldr     x17, [x16]
     *  848:   91000210        add     x16, x16, #0x0
     *  84c:   d61f0220        br      x17
     *
     * If the symbol isn't resolved, the return address is in lr (x30) register
     * and on the stack. We need to change both of them.
     */
    if (ptr == &(gprState->lr)) {
      rword *sp = reinterpret_cast<rword *>(gprState->sp);

      for (size_t i = 0; i < SCAN_DISTANCE; ++i) {
        if (sp[i] == returnAddress) {
          QBDI_DEBUG("TransferExecution: Found return address also at {:}",
                     reinterpret_cast<void *>(&(sp[i])));

          ptr2 = &(sp[i]);
          break;
        }
      }
    }
  }
  rword hook;

  if (ptr == &(gprState->lr)) {
    hook = archData.transfertLR.hook;
    QBDI_DEBUG(
        "TransferExecution: Used LR as a jumpRegister. "
        "Return address 0x{:} replace with 0x{:x}",
        returnAddress, hook);
    transferBlock->selectSeq(archData.transfertLR.seqID);
  } else {
    hook = archData.transfertX28.hook;
    QBDI_DEBUG(
        "TransferExecution: Used x28 as a jumpRegister. "
        "Return address 0x{:} replace with 0x{:x}",
        returnAddress, hook);
    transferBlock->selectSeq(archData.transfertX28.seqID);
  }

  // set fake return address
  *ptr = hook;
  if (ptr2 != nullptr) {
    *ptr2 = hook;
  }

  // Write transfer state
  transferBlock->getContext()->gprState = *gprState;
  transferBlock->getContext()->fprState = *fprState;

  // Set the jump address in hostate.brokerAddr
  transferBlock->getContext()->hostState.brokerAddr = addr;

  // Execute transfer
  QBDI_DEBUG("Transfering execution to 0x{:x} using transferBlock 0x{:x}", addr,
             reinterpret_cast<uintptr_t>(&*transferBlock));

  transferBlock->run();

  // Read transfer result
  *gprState = transferBlock->getContext()->gprState;
  *fprState = transferBlock->getContext()->fprState;

  // Restore original return
  QBDI_GPR_SET(gprState, REG_PC, returnAddress);
  if (QBDI_GPR_GET(gprState, REG_LR) == hook) {
    QBDI_GPR_SET(gprState, REG_LR, returnAddress);
  }
  if constexpr (is_linux) {
    if (ptr2 != nullptr and *ptr2 == hook) {
      *ptr2 = returnAddress;
    }
  }

  return true;
}

} // namespace QBDI
