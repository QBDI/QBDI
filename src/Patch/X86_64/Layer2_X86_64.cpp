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

#include "MCTargetDesc/X86BaseInfo.h"
#include "X86InstrInfo.h"
#include "llvm/MC/MCInst.h"

#include "Patch/RelocatableInst.h"
#include "Patch/X86_64/Layer2_X86_64.h"
#include "Patch/X86_64/RelocatableInst_X86_64.h"
#include "Utility/LogSys.h"

#include "QBDI/Config.h"

namespace QBDI {

llvm::MCInst mov32rr(RegLLVM dst, RegLLVM src) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::MOV32rr);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));

  return inst;
}

llvm::MCInst mov32ri(RegLLVM reg, rword imm) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::MOV32ri);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(imm));

  return inst;
}

llvm::MCInst mov32mr(RegLLVM base, rword scale, RegLLVM offset,
                     rword displacement, RegLLVM seg, RegLLVM src) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::MOV32mr);
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(scale));
  inst.addOperand(llvm::MCOperand::createReg(offset.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(displacement));
  inst.addOperand(llvm::MCOperand::createReg(seg.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));

  return inst;
}

llvm::MCInst mov32rm8(RegLLVM dst, RegLLVM base, rword scale, RegLLVM offset,
                      rword displacement, RegLLVM seg) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::MOVZX32rm8);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(scale));
  inst.addOperand(llvm::MCOperand::createReg(offset.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(displacement));
  inst.addOperand(llvm::MCOperand::createReg(seg.getValue()));

  return inst;
}

llvm::MCInst mov32rm16(RegLLVM dst, RegLLVM base, rword scale, RegLLVM offset,
                       rword displacement, RegLLVM seg) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::MOVZX32rm16);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(scale));
  inst.addOperand(llvm::MCOperand::createReg(offset.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(displacement));
  inst.addOperand(llvm::MCOperand::createReg(seg.getValue()));

  return inst;
}

llvm::MCInst mov32rm(RegLLVM dst, RegLLVM base, rword scale, RegLLVM offset,
                     rword displacement, RegLLVM seg) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::MOV32rm);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(scale));
  inst.addOperand(llvm::MCOperand::createReg(offset.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(displacement));
  inst.addOperand(llvm::MCOperand::createReg(seg.getValue()));

  return inst;
}

llvm::MCInst movzx32rr8(RegLLVM dst, RegLLVM src) {

  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::MOVZX32rr8);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));

  return inst;
}

llvm::MCInst mov64rr(RegLLVM dst, RegLLVM src) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::MOV64rr);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));

  return inst;
}

llvm::MCInst mov64ri(RegLLVM reg, rword imm) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::MOV64ri);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(imm));

  return inst;
}

llvm::MCInst mov64ri32(RegLLVM reg, rword imm) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::MOV64ri32);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(imm));

  return inst;
}

llvm::MCInst mov64mr(RegLLVM base, rword scale, RegLLVM offset,
                     rword displacement, RegLLVM seg, RegLLVM src) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::MOV64mr);
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(scale));
  inst.addOperand(llvm::MCOperand::createReg(offset.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(displacement));
  inst.addOperand(llvm::MCOperand::createReg(seg.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));

  return inst;
}

llvm::MCInst mov64rm(RegLLVM dst, RegLLVM base, rword scale, RegLLVM offset,
                     rword displacement, RegLLVM seg) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::MOV64rm);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(scale));
  inst.addOperand(llvm::MCOperand::createReg(offset.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(displacement));
  inst.addOperand(llvm::MCOperand::createReg(seg.getValue()));

  return inst;
}

llvm::MCInst movzx64rr8(RegLLVM dst, RegLLVM src) {

  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::MOVZX64rr8);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));

  return inst;
}

llvm::MCInst test32ri(RegLLVM base, uint32_t imm) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::TEST32ri);
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(imm));

  return inst;
}

llvm::MCInst test64ri32(RegLLVM base, uint32_t imm) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::TEST64ri32);
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(imm));

  return inst;
}

llvm::MCInst jmp32m(RegLLVM base, rword offset) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::JMP32m);
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(1));
  inst.addOperand(llvm::MCOperand::createReg(0));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createReg(0));

  return inst;
}

llvm::MCInst jmp64m(RegLLVM base, rword offset) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::JMP64m);
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(1));
  inst.addOperand(llvm::MCOperand::createReg(0));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createReg(0));

  return inst;
}

llvm::MCInst je(int32_t offset) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::JCC_4);
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(llvm::X86::CondCode::COND_E));

  return inst;
}

llvm::MCInst jne(int32_t offset) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::JCC_4);
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(llvm::X86::CondCode::COND_NE));

  return inst;
}

llvm::MCInst jmp(rword offset) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::JMP_4);
  inst.addOperand(llvm::MCOperand::createImm(offset));

  return inst;
}

llvm::MCInst fxsave(RegLLVM base, rword offset) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::FXSAVE);
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(1));
  inst.addOperand(llvm::MCOperand::createReg(0));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createReg(0));

  return inst;
}

llvm::MCInst fxrstor(RegLLVM base, rword offset) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::FXRSTOR);
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(1));
  inst.addOperand(llvm::MCOperand::createReg(0));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createReg(0));

  return inst;
}

llvm::MCInst vextractf128(RegLLVM base, rword offset, RegLLVM src,
                          uint8_t regoffset) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::VEXTRACTF128mr);
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(1));
  inst.addOperand(llvm::MCOperand::createReg(0));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createReg(0));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(regoffset));

  return inst;
}

llvm::MCInst vinsertf128(RegLLVM dst, RegLLVM base, rword offset,
                         uint8_t regoffset) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::VINSERTF128rm);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(1));
  inst.addOperand(llvm::MCOperand::createReg(0));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createReg(0));
  inst.addOperand(llvm::MCOperand::createImm(1));

  return inst;
}

llvm::MCInst push32r(RegLLVM reg) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::PUSH32r);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));

  return inst;
}

llvm::MCInst push64r(RegLLVM reg) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::PUSH64r);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));

  return inst;
}

llvm::MCInst pop32r(RegLLVM reg) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::POP32r);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));

  return inst;
}

llvm::MCInst pop64r(RegLLVM reg) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::POP64r);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));

  return inst;
}

llvm::MCInst addr32i(RegLLVM dst, RegLLVM src, rword imm) {

  // We use LEA to avoid flags to be modified
  return lea32(dst, src, 1, 0, imm, 0);
}

llvm::MCInst addr64i(RegLLVM dst, RegLLVM src, rword imm) {

  // We use LEA to avoid flags to be modified
  return lea64(dst, src, 1, 0, imm, 0);
}

llvm::MCInst lea32(RegLLVM dst, RegLLVM base, rword scale, RegLLVM offset,
                   rword displacement, RegLLVM seg) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::LEA32r);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(scale));
  inst.addOperand(llvm::MCOperand::createReg(offset.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(displacement));
  inst.addOperand(llvm::MCOperand::createReg(seg.getValue()));

  return inst;
}

llvm::MCInst lea64(RegLLVM dst, RegLLVM base, rword scale, RegLLVM offset,
                   rword displacement, RegLLVM seg) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::LEA64r);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(scale));
  inst.addOperand(llvm::MCOperand::createReg(offset.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(displacement));
  inst.addOperand(llvm::MCOperand::createReg(seg.getValue()));

  return inst;
}

llvm::MCInst popf32() {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::POPF32);

  return inst;
}

llvm::MCInst popf64() {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::POPF64);

  return inst;
}

llvm::MCInst pushf32() {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::PUSHF32);

  return inst;
}

llvm::MCInst pushf64() {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::PUSHF64);

  return inst;
}

llvm::MCInst ret() {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::RET64);

  return inst;
}

llvm::MCInst rdfsbase64(RegLLVM reg) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::RDFSBASE64);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));

  return inst;
}

llvm::MCInst rdgsbase64(RegLLVM reg) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::RDGSBASE64);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));

  return inst;
}

llvm::MCInst wrfsbase64(RegLLVM reg) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::WRFSBASE64);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));

  return inst;
}

llvm::MCInst wrgsbase64(RegLLVM reg) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::WRGSBASE64);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));

  return inst;
}

llvm::MCInst nop() {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::NOOP);

  return inst;
}

llvm::MCInst xor32rr(RegLLVM dst, RegLLVM src) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::XOR32rr);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));

  return inst;
}

llvm::MCInst xor64rr(RegLLVM dst, RegLLVM src) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::X86::XOR64rr);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));

  return inst;
}

// high level layer 2

[[maybe_unused]] static bool isr8_15Reg(RegLLVM r) {
  switch (r.getValue()) {
    default:
      return false;
    case llvm::X86::R8:
    case llvm::X86::R9:
    case llvm::X86::R10:
    case llvm::X86::R11:
    case llvm::X86::R12:
    case llvm::X86::R13:
    case llvm::X86::R14:
    case llvm::X86::R15:
      return true;
  }
}

static unsigned lenInstLEAtype(RegLLVM base, RegLLVM scale, Constant cst,
                               RegLLVM segment) {

  sword imm = cst;

  if (segment != 0) {
    return lenInstLEAtype(base, scale, cst, 0) + 1;
  } else if (base == 0 and scale == 0) {
    return (is_x86_64) ? 8 : 6;
  } else if (base == llvm::X86::RIP) {
    return 7;
  } else if (base == 0) {
    return (is_x86_64) ? 8 : 7;
  } else {
    unsigned len = (is_x86_64) ? 2 : 1;
    if (scale != 0 or base == llvm::X86::ESP or base == llvm::X86::RSP or
        base == llvm::X86::R12) {
      len++;
    }
    if ((imm < -128) or (128 <= imm)) {
      len = len + 5;
    } else if (imm != 0 or base == llvm::X86::EBP or base == llvm::X86::RBP or
               base == llvm::X86::R13) {
      len = len + 2;
    } else {
      len++;
    }
    return len;
  }
}

static inline RelocatableInst::UniquePtr
DataBlockRelx86(llvm::MCInst &&inst, unsigned int opn, rword offset,
                unsigned size64, unsigned size32) {
  QBDI_REQUIRE_ABORT(opn < inst.getNumOperands(), "Invalid offset {}", opn);
  QBDI_REQUIRE_ABORT(inst.getOperand(opn).isReg(), "Invalid operand type");
  if constexpr (is_x86_64) {
    inst.getOperand(opn /* AddrBaseReg */).setReg(GPR_ID[REG_PC].getValue());
    return DataBlockRel::unique(std::forward<llvm::MCInst>(inst),
                                opn + 3 /* AddrDisp */, offset - size64,
                                size64);
  } else {
    inst.getOperand(opn /* AddrBaseReg */).setReg(0);
    return DataBlockAbsRel::unique(std::forward<llvm::MCInst>(inst),
                                   opn + 3 /* AddrDisp */, offset, size32);
  }
}

RelocatableInst::UniquePtr JmpM(Offset offset) {
  if constexpr (is_x86_64)
    return DataBlockRel::unique(jmp64m(Reg(REG_PC), 0), 3, offset - 6, 6);
  else
    return DataBlockAbsRel::unique(jmp32m(0, 0), 3, offset, 6);
}

RelocatableInst::UniquePtr Fxsave(Offset offset) {
  return DataBlockRelx86(fxsave(0, 0), 0, offset, 7, 7);
}

RelocatableInst::UniquePtr Fxrstor(Offset offset) {
  return DataBlockRelx86(fxrstor(0, 0), 0, offset, 7, 7);
}

RelocatableInst::UniquePtr Vextractf128(Offset offset, RegLLVM src,
                                        Constant regoffset) {
  return DataBlockRelx86(vextractf128(0, 0, src, regoffset), 0, offset, 10, 10);
}

RelocatableInst::UniquePtr Vinsertf128(RegLLVM dst, Offset offset,
                                       Constant regoffset) {
  return DataBlockRelx86(vinsertf128(dst, 0, 0, regoffset), 2, offset, 10, 10);
}

RelocatableInst::UniquePtr Pushr(Reg reg) {
  if constexpr (is_x86_64)
    return NoRelocSized::unique(push64r(reg), isr8_15Reg(reg) ? 2 : 1);
  else
    return NoRelocSized::unique(push32r(reg), 1);
}

RelocatableInst::UniquePtr Popr(Reg reg) {
  if constexpr (is_x86_64)
    return NoRelocSized::unique(pop64r(reg), isr8_15Reg(reg) ? 2 : 1);
  else
    return NoRelocSized::unique(pop32r(reg), 1);
}

RelocatableInst::UniquePtr Add(Reg dest, Reg src, Constant cst) {
  if constexpr (is_x86_64)
    return NoRelocSized::unique(addr64i(dest, src, cst),
                                lenInstLEAtype(src, 0, cst, 0));
  else
    return NoRelocSized::unique(addr32i(dest, src, cst),
                                lenInstLEAtype(src, 0, cst, 0));
}

RelocatableInst::UniquePtr Pushf() {
  if constexpr (is_x86_64)
    return NoRelocSized::unique(pushf64(), 1);
  else
    return NoRelocSized::unique(pushf32(), 1);
}

RelocatableInst::UniquePtr Popf() {
  if constexpr (is_x86_64)
    return NoRelocSized::unique(popf64(), 1);
  else
    return NoRelocSized::unique(popf32(), 1);
}

RelocatableInst::UniquePtr Ret() { return NoRelocSized::unique(ret(), 1); }

RelocatableInst::UniquePtr Test(Reg reg, uint32_t value) {
  if constexpr (is_x86_64)
    return NoRelocSized::unique(test64ri32(reg, value), 7);
  else
    return NoRelocSized::unique(test32ri(reg, value), 6);
}

RelocatableInst::UniquePtr Je(int32_t offset) {
  return NoRelocSized::unique(je(offset), 6);
}

RelocatableInst::UniquePtr Jne(int32_t offset) {
  return NoRelocSized::unique(jne(offset), 6);
}

RelocatableInst::UniquePtr Rdfsbase(Reg reg) {
  return NoRelocSized::unique(rdfsbase64(reg), 5);
}

RelocatableInst::UniquePtr Rdgsbase(Reg reg) {
  return NoRelocSized::unique(rdgsbase64(reg), 5);
}

RelocatableInst::UniquePtr Wrfsbase(Reg reg) {
  return NoRelocSized::unique(wrfsbase64(reg), 5);
}

RelocatableInst::UniquePtr Wrgsbase(Reg reg) {
  return NoRelocSized::unique(wrgsbase64(reg), 5);
}

RelocatableInst::UniquePtr Xorrr(RegLLVM dst, RegLLVM src) {
  if constexpr (is_x86_64)
    return NoRelocSized::unique(xor64rr(dst, src), 3);
  else
    return NoRelocSized::unique(xor32rr(dst, src), 2);
}

RelocatableInst::UniquePtr Lea(RegLLVM dst, RegLLVM base, rword scale,
                               RegLLVM offset, rword disp, RegLLVM seg) {
  if (base == 0 and scale == 1 and offset != 0) {
    base = offset;
    offset = 0;
  }
  if constexpr (is_x86_64)
    return NoRelocSized::unique(lea64(dst, base, scale, offset, disp, seg),
                                lenInstLEAtype(base, offset, disp, seg));
  else
    return NoRelocSized::unique(lea32(dst, base, scale, offset, disp, seg),
                                lenInstLEAtype(base, offset, disp, seg));
}

RelocatableInst::UniquePtr MovzxrAL(Reg dst) {
  if constexpr (is_x86_64)
    return NoRelocSized::unique(movzx64rr8(dst, llvm::X86::AL), 4);
  else
    return NoRelocSized::unique(movzx32rr8(dst, llvm::X86::AL), 3);
}

RelocatableInst::UniquePtr Mov64rm(RegLLVM dst, RegLLVM addr, RegLLVM seg) {
  return NoRelocSized::unique(mov64rm(dst, addr, 1, 0, 0, seg),
                              lenInstLEAtype(addr, 0, 0, seg));
}

RelocatableInst::UniquePtr Mov32rm(RegLLVM dst, RegLLVM addr, RegLLVM seg) {
  unsigned len = 2;
  if (seg != 0) {
    len++;
  }
  if constexpr (is_x86_64) {
    if (isr8_15Reg(addr) or addr == llvm::X86::RSP or addr == llvm::X86::RBP) {
      len++;
    }
  } else {
    if (addr == llvm::X86::ESP or addr == llvm::X86::EBP) {
      len++;
    }
  }
  return NoRelocSized::unique(mov32rm(dst, addr, 1, 0, 0, seg), len);
}

RelocatableInst::UniquePtr Mov32rm16(RegLLVM dst, RegLLVM addr, RegLLVM seg) {
  unsigned len = 3;
  if (seg != 0) {
    len++;
  }
  if constexpr (is_x86_64) {
    if (isr8_15Reg(addr) or addr == llvm::X86::RSP or addr == llvm::X86::RBP) {
      len++;
    }
  } else {
    if (addr == llvm::X86::ESP or addr == llvm::X86::EBP) {
      len++;
    }
  }
  return NoRelocSized::unique(mov32rm16(dst, addr, 1, 0, 0, seg), len);
}

RelocatableInst::UniquePtr Mov32rm8(RegLLVM dst, RegLLVM addr, RegLLVM seg) {
  unsigned len = 3;
  if (seg != 0) {
    len++;
  }
  if constexpr (is_x86_64) {
    if (isr8_15Reg(addr) or addr == llvm::X86::RSP or addr == llvm::X86::RBP) {
      len++;
    }
  } else {
    if (addr == llvm::X86::ESP or addr == llvm::X86::EBP) {
      len++;
    }
  }
  return NoRelocSized::unique(mov32rm8(dst, addr, 1, 0, 0, seg), len);
}

} // namespace QBDI
