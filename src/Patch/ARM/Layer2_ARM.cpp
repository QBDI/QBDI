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
#include "Patch/ARM/Layer2_ARM.h"
#include "Patch/ARM/RelocatableInst_ARM.h"
#include "Utility/LogSys.h"

#include "Target/ARM/ARMSubtarget.h"
#include "Target/ARM/MCTargetDesc/ARMAddressingModes.h"

namespace QBDI {

static inline unsigned int getCondReg(unsigned cond) {
  if (cond == llvm::ARMCC::AL) {
    return llvm::ARM::NoRegister;
  } else {
    return llvm::ARM::CPSR;
  }
}

static inline void ldmstmCommon(llvm::MCInst &inst, RegLLVM base,
                                unsigned int regMask, unsigned cond) {
  QBDI_REQUIRE_ABORT(regMask != 0, "Empty register list");
  QBDI_REQUIRE_ABORT((regMask >> 16) == 0,
                     "Unsupported register in list mask: 0x{:x}", regMask);

  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  for (int i = 0; i < 16; i++) {
    if ((regMask & (1 << i)) != 0) {
      switch (i) {
        default:
          inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::R0 + i));
          break;
        case 13:
          inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::SP));
          break;
        case 14:
          inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::LR));
          break;
        case 15:
          inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::PC));
          break;
      }
    }
  }
}

// utils

bool armExpandCompatible(rword imm) {
  return llvm::ARM_AM::getSOImmVal(imm) != -1;
}

bool thumbExpandCompatible(rword imm) {
  return llvm::ARM_AM::getT2SOImmValSplatVal(imm) != -1;
}

bool t2moviCompatible(rword imm) {
  if (imm < 256) {
    return true;
  }
  for (int i = 0; i < 24; i++) {
    rword mask = (1 << (i)) - 1;
    if ((imm & mask) != 0) {
      return false;
    }
    if ((imm >> i) < 256) {
      return true;
    }
  }
  return false;
}

// LoadInst 4

llvm::MCInst popr1(RegLLVM reg, unsigned cond) {
  return ldrPost(reg, llvm::ARM::SP, 4, cond);
}

llvm::MCInst ldri12(RegLLVM reg, RegLLVM base, sword offset) {
  return ldri12(reg, base, offset, llvm::ARMCC::AL);
}

llvm::MCInst ldri12(RegLLVM reg, RegLLVM base, sword offset, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(-4096 < offset and offset < 4096,
                     "offset not in the range [-4095, 4095] ({})", offset);

  inst.setOpcode(llvm::ARM::LDRi12);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

llvm::MCInst ldrPost(RegLLVM dst, RegLLVM src, sword offset) {
  return ldrPost(dst, src, offset, llvm::ARMCC::AL);
}

llvm::MCInst ldrPost(RegLLVM dst, RegLLVM src, sword offset, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(-4096 < offset and offset < 4096,
                     "offset not in the range [-4095, 4095] ({})", offset);

  inst.setOpcode(llvm::ARM::LDR_POST_IMM);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::NoRegister));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

llvm::MCInst t2ldri8(RegLLVM reg, RegLLVM base, sword offset) {
  return t2ldri8(reg, base, offset, llvm::ARMCC::AL);
}

llvm::MCInst t2ldri8(RegLLVM reg, RegLLVM base, sword offset, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(-256 < offset and offset < 256,
                     "offset not in the range [-255, 255] ({})", offset);

  inst.setOpcode(llvm::ARM::t2LDRi8);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

llvm::MCInst t2ldri12(RegLLVM reg, RegLLVM base, sword offset) {
  return t2ldri12(reg, base, offset, llvm::ARMCC::AL);
}

llvm::MCInst t2ldri12(RegLLVM reg, RegLLVM base, sword offset, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(0 <= offset and offset < 4096,
                     "offset not in the range [0, 4095] ({})", offset);

  inst.setOpcode(llvm::ARM::t2LDRi12);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

llvm::MCInst t2ldrPost(RegLLVM reg, RegLLVM base, sword offset) {
  return t2ldrPost(reg, base, offset, llvm::ARMCC::AL);
}

llvm::MCInst t2ldrPost(RegLLVM reg, RegLLVM base, sword offset, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(-256 < offset and offset < 256,
                     "offset not in the range [-255, 255] ({})", offset);

  inst.setOpcode(llvm::ARM::t2LDR_POST);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

llvm::MCInst t2ldrPre(RegLLVM reg, RegLLVM base, sword offset) {
  return t2ldrPre(reg, base, offset, llvm::ARMCC::AL);
}

llvm::MCInst t2ldrPre(RegLLVM reg, RegLLVM base, sword offset, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(-256 < offset and offset < 256,
                     "offset not in the range [-255, 255] ({})", offset);

  inst.setOpcode(llvm::ARM::t2LDR_PRE);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

// LoadInst 2

llvm::MCInst ldrh(RegLLVM dst, RegLLVM src, unsigned int offset) {
  return ldrh(dst, src, offset, llvm::ARMCC::AL);
}

llvm::MCInst ldrh(RegLLVM dst, RegLLVM src, unsigned int offset,
                  unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::LDRH);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(0));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

llvm::MCInst ldrhPost(RegLLVM dst, RegLLVM src) {
  return ldrhPost(dst, src, llvm::ARMCC::AL);
}

llvm::MCInst ldrhPost(RegLLVM dst, RegLLVM src, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::LDRH_POST);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(0));
  inst.addOperand(llvm::MCOperand::createImm(2));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

llvm::MCInst t2ldrh(RegLLVM dst, RegLLVM src, unsigned int offset) {
  return t2ldrh(dst, src, offset, llvm::ARMCC::AL);
}

llvm::MCInst t2ldrh(RegLLVM dst, RegLLVM src, unsigned int offset,
                    unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::t2LDRHi12);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

llvm::MCInst t2ldrhPost(RegLLVM dst, RegLLVM src) {
  return t2ldrhPost(dst, src, llvm::ARMCC::AL);
}

llvm::MCInst t2ldrhPost(RegLLVM dst, RegLLVM src, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::t2LDRH_POST);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(2));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

llvm::MCInst t2ldrhr(RegLLVM base, RegLLVM reg, RegLLVM offReg) {
  return t2ldrhrs(base, reg, offReg, 0, llvm::ARMCC::AL);
}

llvm::MCInst t2ldrhr(RegLLVM base, RegLLVM reg, RegLLVM offReg, unsigned cond) {
  return t2ldrhrs(base, reg, offReg, 0, cond);
}

llvm::MCInst t2ldrhrs(RegLLVM base, RegLLVM reg, RegLLVM offReg, unsigned lsl) {
  return t2ldrhrs(base, reg, offReg, lsl, llvm::ARMCC::AL);
}

llvm::MCInst t2ldrhrs(RegLLVM base, RegLLVM reg, RegLLVM offReg, unsigned lsl,
                      unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(lsl <= 4, "Invalid shift value: {}", lsl);

  inst.setOpcode(llvm::ARM::t2LDRHs);
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(offReg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(lsl));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

// LoadInst 1

llvm::MCInst ldrb(RegLLVM dst, RegLLVM src, unsigned int offset) {
  return ldrb(dst, src, offset, llvm::ARMCC::AL);
}

llvm::MCInst ldrb(RegLLVM dst, RegLLVM src, unsigned int offset,
                  unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::LDRBi12);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

llvm::MCInst ldrbPost(RegLLVM dst, RegLLVM src) {
  return ldrbPost(dst, src, llvm::ARMCC::AL);
}

llvm::MCInst ldrbPost(RegLLVM dst, RegLLVM src, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::LDRB_POST_IMM);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(0));
  inst.addOperand(llvm::MCOperand::createImm(1));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

llvm::MCInst t2ldrb(RegLLVM dst, RegLLVM src, unsigned int offset) {
  return t2ldrb(dst, src, offset, llvm::ARMCC::AL);
}

llvm::MCInst t2ldrb(RegLLVM dst, RegLLVM src, unsigned int offset,
                    unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::t2LDRBi12);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

llvm::MCInst t2ldrbPost(RegLLVM dst, RegLLVM src) {
  return t2ldrbPost(dst, src, llvm::ARMCC::AL);
}

llvm::MCInst t2ldrbPost(RegLLVM dst, RegLLVM src, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::t2LDRB_POST);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(1));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

llvm::MCInst t2ldrbr(RegLLVM base, RegLLVM reg, RegLLVM offReg) {
  return t2ldrbrs(base, reg, offReg, 0, llvm::ARMCC::AL);
}

llvm::MCInst t2ldrbr(RegLLVM base, RegLLVM reg, RegLLVM offReg, unsigned cond) {
  return t2ldrbrs(base, reg, offReg, 0, cond);
}

llvm::MCInst t2ldrbrs(RegLLVM base, RegLLVM reg, RegLLVM offReg, unsigned lsl) {
  return t2ldrbrs(base, reg, offReg, lsl, llvm::ARMCC::AL);
}

llvm::MCInst t2ldrbrs(RegLLVM base, RegLLVM reg, RegLLVM offReg, unsigned lsl,
                      unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(lsl <= 4, "Invalid shift value: {}", lsl);

  inst.setOpcode(llvm::ARM::t2LDRBs);
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(offReg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(lsl));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

// load exclusif

llvm::MCInst ldrexb(RegLLVM dest, RegLLVM reg) {
  return ldrexb(dest, reg, llvm::ARMCC::AL);
}

llvm::MCInst ldrexb(RegLLVM dest, RegLLVM reg, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::LDREXB);
  inst.addOperand(llvm::MCOperand::createReg(dest.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

llvm::MCInst t2ldrexb(RegLLVM dest, RegLLVM reg) {
  return t2ldrexb(dest, reg, llvm::ARMCC::AL);
}

llvm::MCInst t2ldrexb(RegLLVM dest, RegLLVM reg, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::t2LDREXB);
  inst.addOperand(llvm::MCOperand::createReg(dest.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

llvm::MCInst ldrexh(RegLLVM dest, RegLLVM reg) {
  return ldrexh(dest, reg, llvm::ARMCC::AL);
}

llvm::MCInst ldrexh(RegLLVM dest, RegLLVM reg, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::LDREXH);
  inst.addOperand(llvm::MCOperand::createReg(dest.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

llvm::MCInst t2ldrexh(RegLLVM dest, RegLLVM reg) {
  return t2ldrexh(dest, reg, llvm::ARMCC::AL);
}

llvm::MCInst t2ldrexh(RegLLVM dest, RegLLVM reg, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::t2LDREXH);
  inst.addOperand(llvm::MCOperand::createReg(dest.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

llvm::MCInst ldrex(RegLLVM dest, RegLLVM reg) {
  return ldrex(dest, reg, llvm::ARMCC::AL);
}

llvm::MCInst ldrex(RegLLVM dest, RegLLVM reg, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::LDREX);
  inst.addOperand(llvm::MCOperand::createReg(dest.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

llvm::MCInst t2ldrex(RegLLVM dest, RegLLVM reg) {
  return t2ldrex(dest, reg, llvm::ARMCC::AL);
}

llvm::MCInst t2ldrex(RegLLVM dest, RegLLVM reg, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::t2LDREX);
  inst.addOperand(llvm::MCOperand::createReg(dest.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(0));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

llvm::MCInst ldrexd(RegLLVM dest, RegLLVM dest2, RegLLVM reg) {
  return ldrexd(dest, dest2, reg, llvm::ARMCC::AL);
}

llvm::MCInst ldrexd(RegLLVM dest, RegLLVM dest2, RegLLVM reg, unsigned cond) {
  llvm::MCInst inst;
  unsigned coupleReg = 0;

  QBDI_REQUIRE_ABORT(dest2.getValue() - dest.getValue() == 1,
                     "Need consecutive register");

  switch (dest.getValue()) {
    case llvm::ARM::R0:
      coupleReg = llvm::ARM::R0_R1;
      break;
    case llvm::ARM::R2:
      coupleReg = llvm::ARM::R2_R3;
      break;
    case llvm::ARM::R4:
      coupleReg = llvm::ARM::R4_R5;
      break;
    case llvm::ARM::R6:
      coupleReg = llvm::ARM::R6_R7;
      break;
    case llvm::ARM::R8:
      coupleReg = llvm::ARM::R8_R9;
      break;
    case llvm::ARM::R10:
      coupleReg = llvm::ARM::R10_R11;
      break;
    default:
      QBDI_ABORT("Invalid destination register");
  }

  inst.setOpcode(llvm::ARM::LDREXD);
  inst.addOperand(llvm::MCOperand::createReg(coupleReg));
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

llvm::MCInst t2ldrexd(RegLLVM dest, RegLLVM dest2, RegLLVM reg) {
  return t2ldrexd(dest, dest2, reg, llvm::ARMCC::AL);
}

llvm::MCInst t2ldrexd(RegLLVM dest, RegLLVM dest2, RegLLVM reg, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::t2LDREXD);
  inst.addOperand(llvm::MCOperand::createReg(dest.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(dest2.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

// load multiple

llvm::MCInst ldmia(RegLLVM base, unsigned int regMask, bool wback) {
  return ldmia(base, regMask, wback, llvm::ARMCC::AL);
}

llvm::MCInst ldmia(RegLLVM base, unsigned int regMask, bool wback,
                   unsigned cond) {
  llvm::MCInst inst;

  if (wback) {
    inst.setOpcode(llvm::ARM::LDMIA_UPD);
    inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  } else {
    inst.setOpcode(llvm::ARM::LDMIA);
  }
  ldmstmCommon(inst, base, regMask, cond);
  return inst;
}

llvm::MCInst t2ldmia(RegLLVM base, unsigned int regMask, bool wback) {
  return t2ldmia(base, regMask, wback, llvm::ARMCC::AL);
}

llvm::MCInst t2ldmia(RegLLVM base, unsigned int regMask, bool wback,
                     unsigned cond) {
  llvm::MCInst inst;
  // SP forbidden in thumb mode
  QBDI_REQUIRE_ABORT(((regMask >> 13) & 1) == 0,
                     "SP forbidden in thumb mode (regMask : 0x{:x}", regMask);

  if (wback) {
    inst.setOpcode(llvm::ARM::t2LDMIA_UPD);
    inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  } else {
    inst.setOpcode(llvm::ARM::t2LDMIA);
  }
  ldmstmCommon(inst, base, regMask, cond);
  return inst;
}

llvm::MCInst t2ldmdb(RegLLVM base, unsigned int regMask, bool wback) {
  return t2ldmdb(base, regMask, wback, llvm::ARMCC::AL);
}

llvm::MCInst t2ldmdb(RegLLVM base, unsigned int regMask, bool wback,
                     unsigned cond) {
  llvm::MCInst inst;
  // SP forbidden in thumb mode
  QBDI_REQUIRE_ABORT(((regMask >> 13) & 1) == 0,
                     "SP forbidden in thumb mode (regMask : 0x{:x}", regMask);

  if (wback) {
    inst.setOpcode(llvm::ARM::t2LDMDB_UPD);
    inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  } else {
    inst.setOpcode(llvm::ARM::t2LDMDB);
  }
  ldmstmCommon(inst, base, regMask, cond);
  return inst;
}

llvm::MCInst vldmia(RegLLVM base, RegLLVM reg, unsigned int nreg, bool wback) {
  return vldmia(base, reg, nreg, wback, llvm::ARMCC::AL);
}

llvm::MCInst vldmia(RegLLVM base, RegLLVM reg, unsigned int nreg, bool wback,
                    unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(0 < nreg and nreg <= 16,
                     "Invalid number of register to load: {}", nreg);
  QBDI_REQUIRE_ABORT(llvm::ARM::D0 <= reg.getValue() and
                         reg.getValue() + (nreg - 1) <= llvm::ARM::D31,
                     "Invalid register to load (D{} ({}), {})",
                     reg.getValue() - llvm::ARM::D0, reg.getValue(), nreg);

  if (wback) {
    inst.setOpcode(llvm::ARM::VLDMDIA_UPD);
    inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  } else {
    inst.setOpcode(llvm::ARM::VLDMDIA);
  }
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  for (unsigned int i = 0; i < nreg; i++) {
    inst.addOperand(llvm::MCOperand::createReg(reg.getValue() + i));
  }
  return inst;
}

// StoreInst

llvm::MCInst pushr1(RegLLVM reg, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::STR_PRE_IMM);
  inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::SP));
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::SP));
  inst.addOperand(llvm::MCOperand::createImm(-4));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

llvm::MCInst stri12(RegLLVM reg, RegLLVM base, sword offset) {
  return stri12(reg, base, offset, llvm::ARMCC::AL);
}

llvm::MCInst stri12(RegLLVM reg, RegLLVM base, sword offset, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(-4096 < offset and offset < 4096,
                     "offset not in the range [-4095, 4095] ({})", offset);

  QBDI_REQUIRE_ABORT(reg != llvm::ARM::PC, "Source register cannot be PC");

  inst.setOpcode(llvm::ARM::STRi12);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

llvm::MCInst t2stri8(RegLLVM reg, RegLLVM base, sword offset) {
  return t2stri8(reg, base, offset, llvm::ARMCC::AL);
}

llvm::MCInst t2stri8(RegLLVM reg, RegLLVM base, sword offset, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(-256 < offset and offset < 256,
                     "offset not in the range [-255, 255] ({})", offset);

  // llvm allows to create t2stri8 with PC, but the instruction is marked as
  // UNDEFINED (for base == PC) or UNPREDICTABLE (for reg == PC) in the
  // documentation
  QBDI_REQUIRE_ABORT(base != llvm::ARM::PC, "Base register cannot be PC");
  QBDI_REQUIRE_ABORT(reg != llvm::ARM::PC, "Source register cannot be PC");

  inst.setOpcode(llvm::ARM::t2STRi8);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

llvm::MCInst t2stri12(RegLLVM reg, RegLLVM base, sword offset) {
  return t2stri12(reg, base, offset, llvm::ARMCC::AL);
}

llvm::MCInst t2stri12(RegLLVM reg, RegLLVM base, sword offset, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(0 <= offset and offset < 4096,
                     "offset not in the range [0, 4095] ({})", offset);

  // llvm allows to create t2stri12 with PC, but the instruction is marked as
  // UNDEFINED (for base == PC) or UNPREDICTABLE (for reg == PC) in the
  // documentation
  QBDI_REQUIRE_ABORT(base != llvm::ARM::PC, "Base register cannot be PC");
  QBDI_REQUIRE_ABORT(reg != llvm::ARM::PC, "Source register cannot be PC");

  inst.setOpcode(llvm::ARM::t2STRi12);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

llvm::MCInst t2strPost(RegLLVM reg, RegLLVM base, sword offset) {
  return t2strPost(reg, base, offset, llvm::ARMCC::AL);
}

llvm::MCInst t2strPost(RegLLVM reg, RegLLVM base, sword offset, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(-256 < offset and offset < 256,
                     "offset not in the range [-255, 255] ({})", offset);
  QBDI_REQUIRE_ABORT(base != llvm::ARM::PC, "Base register cannot be PC");
  QBDI_REQUIRE_ABORT(reg != llvm::ARM::PC, "Source register cannot be PC");

  inst.setOpcode(llvm::ARM::t2STR_POST);
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

llvm::MCInst t2strPre(RegLLVM reg, RegLLVM base, sword offset) {
  return t2strPre(reg, base, offset, llvm::ARMCC::AL);
}

llvm::MCInst t2strPre(RegLLVM reg, RegLLVM base, sword offset, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(-256 < offset and offset < 256,
                     "offset not in the range [-255, 255] ({})", offset);
  QBDI_REQUIRE_ABORT(base != llvm::ARM::PC, "Base register cannot be PC");
  QBDI_REQUIRE_ABORT(reg != llvm::ARM::PC, "Source register cannot be PC");

  inst.setOpcode(llvm::ARM::t2STR_PRE);
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

// store multiple

llvm::MCInst stmia(RegLLVM base, unsigned int regMask, bool wback) {
  return stmia(base, regMask, wback, llvm::ARMCC::AL);
}

llvm::MCInst stmia(RegLLVM base, unsigned int regMask, bool wback,
                   unsigned cond) {
  llvm::MCInst inst;

  if (wback) {
    inst.setOpcode(llvm::ARM::STMIA_UPD);
    inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  } else {
    inst.setOpcode(llvm::ARM::STMIA);
  }
  ldmstmCommon(inst, base, regMask, cond);
  return inst;
}

llvm::MCInst t2stmia(RegLLVM base, unsigned int regMask, bool wback) {
  return t2stmia(base, regMask, wback, llvm::ARMCC::AL);
}

llvm::MCInst t2stmia(RegLLVM base, unsigned int regMask, bool wback,
                     unsigned cond) {
  llvm::MCInst inst;
  // SP and PC forbidden in thumb mode
  QBDI_REQUIRE_ABORT(((regMask >> 13) & 0x5) == 0,
                     "SP and PC forbidden in thumb mode (regMask : 0x{:x}",
                     regMask);

  if (wback) {
    inst.setOpcode(llvm::ARM::t2STMIA_UPD);
    inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  } else {
    inst.setOpcode(llvm::ARM::t2STMIA);
  }
  ldmstmCommon(inst, base, regMask, cond);
  return inst;
}

llvm::MCInst t2stmdb(RegLLVM base, unsigned int regMask, bool wback) {
  return t2stmdb(base, regMask, wback, llvm::ARMCC::AL);
}

llvm::MCInst t2stmdb(RegLLVM base, unsigned int regMask, bool wback,
                     unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(((regMask >> 13) & 0x5) == 0,
                     "SP and PC forbidden in thumb mode (regMask : 0x{:x}",
                     regMask);

  if (wback) {
    inst.setOpcode(llvm::ARM::t2STMDB_UPD);
    inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  } else {
    inst.setOpcode(llvm::ARM::t2STMDB);
  }
  ldmstmCommon(inst, base, regMask, cond);
  return inst;
}

llvm::MCInst vstmia(RegLLVM base, RegLLVM reg, unsigned int nreg, bool wback) {
  return vstmia(base, reg, nreg, wback, llvm::ARMCC::AL);
}

llvm::MCInst vstmia(RegLLVM base, RegLLVM reg, unsigned int nreg, bool wback,
                    unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(0 < nreg and nreg <= 16,
                     "Invalid number of register to store: {}", nreg);
  QBDI_REQUIRE_ABORT(llvm::ARM::D0 <= reg.getValue() and
                         reg.getValue() + (nreg - 1) <= llvm::ARM::D31,
                     "Invalid register to store (D{} ({}), {})",
                     reg.getValue() - llvm::ARM::D0, reg.getValue(), nreg);

  if (wback) {
    inst.setOpcode(llvm::ARM::VSTMDIA_UPD);
    inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  } else {
    inst.setOpcode(llvm::ARM::VSTMDIA);
  }
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  for (unsigned int i = 0; i < nreg; i++) {
    inst.addOperand(llvm::MCOperand::createReg(reg.getValue() + i));
  }
  return inst;
}

// mov register - register

llvm::MCInst movr(RegLLVM dst, RegLLVM src) {
  return movr(dst, src, llvm::ARMCC::AL);
}

llvm::MCInst movr(RegLLVM dst, RegLLVM src, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::MOVr_TC);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::NoRegister));

  return inst;
}

llvm::MCInst tmovr(RegLLVM dst, RegLLVM src) {
  return tmovr(dst, src, llvm::ARMCC::AL);
}

llvm::MCInst tmovr(RegLLVM dst, RegLLVM src, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::tMOVr);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

// mov Immediate

llvm::MCInst movi(RegLLVM dst, rword imm) {
  return movi(dst, imm, llvm::ARMCC::AL);
}

llvm::MCInst movi(RegLLVM dst, rword imm, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(imm < 0x10000, "Unsupported immediate 0x{:x}", imm);

  inst.setOpcode(llvm::ARM::MOVi16);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(imm));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

llvm::MCInst t2movi(RegLLVM dst, rword imm) {
  return t2movi(dst, imm, llvm::ARMCC::AL);
}

llvm::MCInst t2movi(RegLLVM dst, rword imm, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(t2moviCompatible(imm), "Incompatible immediate 0x{:x}",
                     imm);

  inst.setOpcode(llvm::ARM::t2MOVi);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(imm));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::NoRegister));

  return inst;
}

// branch

llvm::MCInst branch(sword offset) { return bcc(offset, llvm::ARMCC::AL); }

llvm::MCInst bcc(sword offset, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(offset % 4 == 0, "Invalid alignemnt 0x{:x}", offset);
  QBDI_REQUIRE_ABORT(-(1 << 26) < offset and offset < (1 << 26),
                     "offset not in the range [-0x3ffffff, 0x3ffffff] (0x{:x})",
                     offset);

  inst.setOpcode(llvm::ARM::Bcc);
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

llvm::MCInst tbranch(sword offset) {
  return tbranchIT(offset, llvm::ARMCC::AL);
}

llvm::MCInst tbranchIT(sword offset, unsigned cond) {
  // if cond != llvm::ARMCC::AL, the instruction must be within an IT block
  // (at the last position)

  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(offset % 2 == 0, "Invalid alignemnt 0x{:x}", offset);
  QBDI_REQUIRE_ABORT(-2048 < offset and offset < 2048,
                     "offset not in the range [-2047, 2047] ({})", offset);

  inst.setOpcode(llvm::ARM::tB);
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

llvm::MCInst tbcc(sword offset, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(offset % 2 == 0, "Invalid alignemnt 0x{:x}", offset);
  QBDI_REQUIRE_ABORT(-256 < offset and offset < 256,
                     "offset not in the range [-255, 255] ({})", offset);
  QBDI_REQUIRE_ABORT(cond != llvm::ARMCC::AL, "Unsupported condition AL");

  inst.setOpcode(llvm::ARM::tBcc);
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

llvm::MCInst t2branch(sword offset) {
  return t2branchIT(offset, llvm::ARMCC::AL);
}

llvm::MCInst t2branchIT(sword offset, unsigned cond) {
  // if cond != llvm::ARMCC::AL, the instruction must be within an IT block
  // (at the last position)

  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(offset % 2 == 0, "Invalid alignemnt 0x{:x}", offset);
  QBDI_REQUIRE_ABORT(-0x1000000 < offset and offset < 0x1000000,
                     "offset not in the range [-0xffffff, 0xffffff] (0x{:x})",
                     offset);

  inst.setOpcode(llvm::ARM::t2B);
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

llvm::MCInst t2bcc(sword offset, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(offset % 2 == 0, "Invalid alignemnt 0x{:x}", offset);
  QBDI_REQUIRE_ABORT(-0x100000 < offset and offset < 0x100000,
                     "offset not in the range [-0xfffff, 0xfffff] (0x{:x})",
                     offset);
  QBDI_REQUIRE_ABORT(cond != llvm::ARMCC::AL, "Unsupported condition AL");

  inst.setOpcode(llvm::ARM::t2Bcc);
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

llvm::MCInst tbx(RegLLVM reg) { return tbx(reg, llvm::ARMCC::AL); }

llvm::MCInst tbx(RegLLVM reg, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::tBX);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

// flags mov

llvm::MCInst mrs(RegLLVM dst) { return mrs(dst, llvm::ARMCC::AL); }

llvm::MCInst mrs(RegLLVM dst, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::MRS);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

llvm::MCInst msr(RegLLVM dst) { return msr(dst, llvm::ARMCC::AL); }

llvm::MCInst msr(RegLLVM dst, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::MSR);
  inst.addOperand(llvm::MCOperand::createImm(/* APSR_nzcvqg */ 0xc));
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

llvm::MCInst t2mrs(RegLLVM dst) { return t2mrs(dst, llvm::ARMCC::AL); }

llvm::MCInst t2mrs(RegLLVM dst, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::t2MRS_AR);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

llvm::MCInst t2msr(RegLLVM dst) { return t2msr(dst, llvm::ARMCC::AL); }

llvm::MCInst t2msr(RegLLVM dst, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::t2MSR_AR);
  inst.addOperand(llvm::MCOperand::createImm(/* APSR_nzcvqg */ 0xc));
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

llvm::MCInst vmrs(RegLLVM dst) { return vmrs(dst, llvm::ARMCC::AL); }

llvm::MCInst vmrs(RegLLVM dst, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::VMRS);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

llvm::MCInst vmsr(RegLLVM dst) { return vmsr(dst, llvm::ARMCC::AL); }

llvm::MCInst vmsr(RegLLVM dst, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::VMSR);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

// get relative address

llvm::MCInst adr(RegLLVM reg, rword offset) {
  return adr(reg, offset, llvm::ARMCC::AL);
}

llvm::MCInst adr(RegLLVM reg, rword offset, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(armExpandCompatible(offset), "Incompatible offset 0x{:x}",
                     offset);

  inst.setOpcode(llvm::ARM::ADR);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

llvm::MCInst t2adr(RegLLVM reg, sword offset) {
  return t2adr(reg, offset, llvm::ARMCC::AL);
}

llvm::MCInst t2adr(RegLLVM reg, sword offset, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(-4096 < offset and offset < 4096,
                     "offset not in the range [-4095, 4095] ({})", offset);

  inst.setOpcode(llvm::ARM::t2ADR);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

// addition

llvm::MCInst add(RegLLVM dst, RegLLVM src, rword imm) {
  return add(dst, src, imm, llvm::ARMCC::AL);
}

llvm::MCInst add(RegLLVM dst, RegLLVM src, rword imm, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(((sword)imm) >= 0, "Invalid immediate 0x{:x}", imm);
  QBDI_REQUIRE_ABORT(armExpandCompatible(imm), "Incompatible immediate 0x{:x}",
                     imm);

  inst.setOpcode(llvm::ARM::ADDri);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(llvm::ARM_AM::getSOImmVal(imm)));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::NoRegister));
  return inst;
}

llvm::MCInst t2add(RegLLVM dst, RegLLVM src, rword imm) {
  return t2add(dst, src, imm, llvm::ARMCC::AL);
}

llvm::MCInst t2add(RegLLVM dst, RegLLVM src, rword imm, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(((sword)imm) >= 0, "Invalid immediate 0x{:x}", imm);
  QBDI_REQUIRE_ABORT(imm < 4096, "Invalid immediate 0x{:x}", imm);
  QBDI_REQUIRE_ABORT(dst != llvm::ARM::SP or src == llvm::ARM::SP,
                     "Dest register can be SP only if it's also the source");

  inst.setOpcode(llvm::ARM::t2ADDri12);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(imm));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

llvm::MCInst addrsi(RegLLVM dst, RegLLVM src, RegLLVM srcOff, unsigned shift,
                    unsigned shiftType) {
  return addrsi(dst, src, srcOff, shift, shiftType, llvm::ARMCC::AL);
}

llvm::MCInst addrsi(RegLLVM dst, RegLLVM src, RegLLVM srcOff, unsigned shift,
                    unsigned shiftType, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(
      (shiftType == llvm::ARM_AM::lsl) or (shiftType == llvm::ARM_AM::lsr) or
          (shiftType == llvm::ARM_AM::asr) or
          (shiftType == llvm::ARM_AM::ror) or (shiftType == llvm::ARM_AM::rrx),
      "Unsupported shift type {}", shiftType);
  QBDI_REQUIRE_ABORT(shift < (1 << 5), "Unsupported shift: 0x{:x}", shift);

  inst.setOpcode(llvm::ARM::ADDrsi);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(srcOff.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(
      llvm::ARM_AM::getSORegOpc((llvm::ARM_AM::ShiftOpc)shiftType, shift)));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::NoRegister));
  return inst;
}

llvm::MCInst t2addrsi(RegLLVM dst, RegLLVM src, RegLLVM srcOff, unsigned shift,
                      unsigned shiftType) {
  return t2addrsi(dst, src, srcOff, shift, shiftType, llvm::ARMCC::AL);
}

llvm::MCInst t2addrsi(RegLLVM dst, RegLLVM src, RegLLVM srcOff, unsigned shift,
                      unsigned shiftType, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(
      (shiftType == llvm::ARM_AM::lsl) or (shiftType == llvm::ARM_AM::lsr) or
          (shiftType == llvm::ARM_AM::asr) or
          (shiftType == llvm::ARM_AM::ror) or (shiftType == llvm::ARM_AM::rrx),
      "Unsupported shift type {}", shiftType);
  QBDI_REQUIRE_ABORT(shift < (1 << 5), "Unsupported shift: 0x{:x}", shift);

  inst.setOpcode(llvm::ARM::t2ADDrs);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(srcOff.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(
      llvm::ARM_AM::getSORegOpc((llvm::ARM_AM::ShiftOpc)shiftType, shift)));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::NoRegister));
  return inst;
}

llvm::MCInst addr(RegLLVM dst, RegLLVM src, RegLLVM src2) {
  return addr(dst, src, src2, llvm::ARMCC::AL);
}

llvm::MCInst addr(RegLLVM dst, RegLLVM src, RegLLVM src2, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::ADDrr);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src2.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::NoRegister));
  return inst;
}

llvm::MCInst t2addr(RegLLVM dst, RegLLVM src, RegLLVM src2) {
  return t2addr(dst, src, src2, llvm::ARMCC::AL);
}

llvm::MCInst t2addr(RegLLVM dst, RegLLVM src, RegLLVM src2, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::t2ADDrr);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src2.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::NoRegister));
  return inst;
}

// substration

llvm::MCInst sub(RegLLVM dst, RegLLVM src, rword imm) {
  return sub(dst, src, imm, llvm::ARMCC::AL);
}

llvm::MCInst sub(RegLLVM dst, RegLLVM src, rword imm, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(((sword)imm) >= 0, "Invalid immediate 0x{:x}", imm);
  QBDI_REQUIRE_ABORT(armExpandCompatible(imm), "Incompatible immediate 0x{:x}",
                     imm);

  inst.setOpcode(llvm::ARM::SUBri);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(llvm::ARM_AM::getSOImmVal(imm)));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::NoRegister));
  return inst;
}

llvm::MCInst t2sub(RegLLVM dst, RegLLVM src, rword imm) {
  return t2sub(dst, src, imm, llvm::ARMCC::AL);
}

llvm::MCInst t2sub(RegLLVM dst, RegLLVM src, rword imm, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(((sword)imm) >= 0, "Invalid immediate 0x{:x}", imm);
  QBDI_REQUIRE_ABORT(imm < 4096, "Invalid immediate 0x{:x}", imm);
  QBDI_REQUIRE_ABORT(dst != llvm::ARM::SP or src == llvm::ARM::SP,
                     "Dest register can be SP only if it's also the source");

  inst.setOpcode(llvm::ARM::t2SUBri12);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(imm));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

llvm::MCInst subrsi(RegLLVM dst, RegLLVM src, RegLLVM srcOff, unsigned shift,
                    unsigned shiftType) {
  return subrsi(dst, src, srcOff, shift, shiftType, llvm::ARMCC::AL);
}

llvm::MCInst subrsi(RegLLVM dst, RegLLVM src, RegLLVM srcOff, unsigned shift,
                    unsigned shiftType, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(
      (shiftType == llvm::ARM_AM::lsl) or (shiftType == llvm::ARM_AM::lsr) or
          (shiftType == llvm::ARM_AM::asr) or
          (shiftType == llvm::ARM_AM::ror) or (shiftType == llvm::ARM_AM::rrx),
      "Unsupported shift type {}", shiftType);
  QBDI_REQUIRE_ABORT(shift < (1 << 5), "Unsupported shift: 0x{:x}", shift);

  inst.setOpcode(llvm::ARM::SUBrsi);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(srcOff.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(shiftType | (shift << 3)));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::NoRegister));
  return inst;
}

llvm::MCInst t2subrsi(RegLLVM dst, RegLLVM src, RegLLVM srcOff, unsigned shift,
                      unsigned shiftType) {
  return t2subrsi(dst, src, srcOff, shift, shiftType, llvm::ARMCC::AL);
}

llvm::MCInst t2subrsi(RegLLVM dst, RegLLVM src, RegLLVM srcOff, unsigned shift,
                      unsigned shiftType, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(
      (shiftType == llvm::ARM_AM::lsl) or (shiftType == llvm::ARM_AM::lsr) or
          (shiftType == llvm::ARM_AM::asr) or
          (shiftType == llvm::ARM_AM::ror) or (shiftType == llvm::ARM_AM::rrx),
      "Unsupported shift type {}", shiftType);
  QBDI_REQUIRE_ABORT(shift < (1 << 5), "Unsupported shift: 0x{:x}", shift);

  inst.setOpcode(llvm::ARM::t2SUBrs);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(srcOff.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(
      llvm::ARM_AM::getSORegOpc((llvm::ARM_AM::ShiftOpc)shiftType, shift)));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::NoRegister));
  return inst;
}

llvm::MCInst subr(RegLLVM dst, RegLLVM src, RegLLVM src2) {
  return subr(dst, src, src2, llvm::ARMCC::AL);
}

llvm::MCInst subr(RegLLVM dst, RegLLVM src, RegLLVM src2, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::SUBrr);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src2.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::NoRegister));
  return inst;
}

llvm::MCInst t2subr(RegLLVM dst, RegLLVM src, RegLLVM src2) {
  return t2subr(dst, src, src2, llvm::ARMCC::AL);
}

llvm::MCInst t2subr(RegLLVM dst, RegLLVM src, RegLLVM src2, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::t2SUBrr);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src2.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::NoRegister));
  return inst;
}

// bit clear

llvm::MCInst bic(RegLLVM dst, RegLLVM src, rword imm) {
  return bic(dst, src, imm, llvm::ARMCC::AL);
}

llvm::MCInst bic(RegLLVM dst, RegLLVM src, rword imm, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(armExpandCompatible(imm), "Incompatible immediate 0x{:x}",
                     imm);

  inst.setOpcode(llvm::ARM::BICri);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(llvm::ARM_AM::getSOImmVal(imm)));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::NoRegister));
  return inst;
}

llvm::MCInst t2bic(RegLLVM dest, RegLLVM reg, rword imm) {
  return t2bic(dest, reg, imm, llvm::ARMCC::AL);
}

llvm::MCInst t2bic(RegLLVM dest, RegLLVM reg, rword imm, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(thumbExpandCompatible(imm),
                     "Incompatible immediate 0x{:x}", imm);

  inst.setOpcode(llvm::ARM::t2BICri);
  inst.addOperand(llvm::MCOperand::createReg(dest.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(imm));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::NoRegister));

  return inst;
}

// or

llvm::MCInst orri(RegLLVM dest, RegLLVM reg, rword imm) {
  return orri(dest, reg, imm, llvm::ARMCC::AL);
}

llvm::MCInst orri(RegLLVM dest, RegLLVM reg, rword imm, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(armExpandCompatible(imm), "Incompatible immediate 0x{:x}",
                     imm);

  inst.setOpcode(llvm::ARM::ORRri);
  inst.addOperand(llvm::MCOperand::createReg(dest.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(llvm::ARM_AM::getSOImmVal(imm)));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::NoRegister));

  return inst;
}

llvm::MCInst t2orri(RegLLVM dest, RegLLVM reg, rword imm) {
  return t2orri(dest, reg, imm, llvm::ARMCC::AL);
}

llvm::MCInst t2orri(RegLLVM dest, RegLLVM reg, rword imm, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(thumbExpandCompatible(imm),
                     "Incompatible immediate 0x{:x}", imm);

  inst.setOpcode(llvm::ARM::t2ORRri);
  inst.addOperand(llvm::MCOperand::createReg(dest.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(imm));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::NoRegister));

  return inst;
}

llvm::MCInst orrshift(RegLLVM dest, RegLLVM reg, RegLLVM reg2,
                      unsigned int lshift) {
  return orrshift(dest, reg, reg2, lshift, llvm::ARMCC::AL);
}

llvm::MCInst orrshift(RegLLVM dest, RegLLVM reg, RegLLVM reg2,
                      unsigned int lshift, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::ORRrsi);
  inst.addOperand(llvm::MCOperand::createReg(dest.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(reg2.getValue()));
  inst.addOperand(
      llvm::MCOperand::createImm((lshift << 3) | llvm::ARM_AM::ShiftOpc::lsl));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  inst.addOperand(llvm::MCOperand::createReg(0));
  return inst;
}

llvm::MCInst t2orrshift(RegLLVM dest, RegLLVM reg, RegLLVM reg2,
                        unsigned int lshift) {
  return t2orrshift(dest, reg, reg2, lshift, llvm::ARMCC::AL);
}

llvm::MCInst t2orrshift(RegLLVM dest, RegLLVM reg, RegLLVM reg2,
                        unsigned int lshift, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::t2ORRrs);
  inst.addOperand(llvm::MCOperand::createReg(dest.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(reg2.getValue()));
  inst.addOperand(
      llvm::MCOperand::createImm((lshift << 3) | llvm::ARM_AM::ShiftOpc::lsl));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  inst.addOperand(llvm::MCOperand::createReg(0));
  return inst;
}

// cmp

llvm::MCInst cmp(RegLLVM src, rword imm) {
  return cmp(src, imm, llvm::ARMCC::AL);
}

llvm::MCInst cmp(RegLLVM src, rword imm, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(armExpandCompatible(imm), "Incompatible immediate 0x{:x}",
                     imm);

  inst.setOpcode(llvm::ARM::CMPri);
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(llvm::ARM_AM::getSOImmVal(imm)));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  return inst;
}

llvm::MCInst t2cmp(RegLLVM reg, rword imm) {
  return t2cmp(reg, imm, llvm::ARMCC::AL);
}

llvm::MCInst t2cmp(RegLLVM reg, rword imm, unsigned cond) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(thumbExpandCompatible(imm),
                     "Incompatible immediate 0x{:x}", imm);

  inst.setOpcode(llvm::ARM::t2CMPri);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(imm));
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));

  return inst;
}

// misc

llvm::MCInst t2it(unsigned int cond, unsigned pred) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::t2IT);
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createImm(pred));

  return inst;
}

llvm::MCInst nop() {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::HINT);
  inst.addOperand(llvm::MCOperand::createImm(0));
  inst.addOperand(llvm::MCOperand::createImm(llvm::ARMCC::AL));
  inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::NoRegister));

  return inst;
}

llvm::MCInst bkpt(unsigned int value) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::BKPT);
  inst.addOperand(llvm::MCOperand::createImm(value));

  return inst;
}

llvm::MCInst t2autg(RegLLVM reg, RegLLVM ctx, RegLLVM tag) {
  return t2autg(reg, ctx, tag, llvm::ARMCC::AL);
}

llvm::MCInst t2autg(RegLLVM reg, RegLLVM ctx, RegLLVM tag, unsigned cond) {
  llvm::MCInst inst;

  inst.setOpcode(llvm::ARM::t2AUTG);
  inst.addOperand(llvm::MCOperand::createImm(cond));
  inst.addOperand(llvm::MCOperand::createReg(getCondReg(cond)));
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(ctx.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(tag.getValue()));

  return inst;
}

// High level layer 2

RelocatableInst::UniquePtr Popr1(CPUMode cpuMode, Reg reg) {
  QBDI_REQUIRE_ABORT(cpuMode == CPUMode::ARM, "Available only in ARM mode");

  return NoReloc::unique(popr1(reg, llvm::ARMCC::AL));
}

RelocatableInst::UniquePtr Pushr1(CPUMode cpuMode, Reg reg) {
  QBDI_REQUIRE_ABORT(cpuMode == CPUMode::ARM, "Available only in ARM mode");

  return NoReloc::unique(pushr1(reg, llvm::ARMCC::AL));
}

RelocatableInst::UniquePtr Add(CPUMode cpuMode, RegLLVM dst, RegLLVM src,
                               Constant cst) {
  if (cpuMode == CPUMode::ARM) {
    if (((sword)cst) < 0) {
      return NoReloc::unique(sub(dst, src, -((sword)cst)));
    } else {
      return NoReloc::unique(add(dst, src, cst));
    }
  } else {
    if (((sword)cst) < 0) {
      return NoReloc::unique(t2sub(dst, src, -((sword)cst)));
    } else {
      return NoReloc::unique(t2add(dst, src, cst));
    }
  }
}

RelocatableInst::UniquePtr Addr(CPUMode cpuMode, RegLLVM dst, RegLLVM src,
                                RegLLVM src2) {
  if (cpuMode == CPUMode::ARM) {
    return NoReloc::unique(addr(dst, src, src2));
  } else {
    return NoReloc::unique(t2addr(dst, src, src2));
  }
}

RelocatableInst::UniquePtr Subr(CPUMode cpuMode, RegLLVM dst, RegLLVM src,
                                RegLLVM src2) {
  if (cpuMode == CPUMode::ARM) {
    return NoReloc::unique(subr(dst, src, src2));
  } else {
    return NoReloc::unique(t2subr(dst, src, src2));
  }
}

RelocatableInst::UniquePtr Addrs(CPUMode cpuMode, RegLLVM dst, RegLLVM src,
                                 RegLLVM srcOff, unsigned shift,
                                 unsigned shiftType) {
  if (cpuMode == CPUMode::ARM) {
    return NoReloc::unique(addrsi(dst, src, srcOff, shift, shiftType));
  } else {
    return NoReloc::unique(t2addrsi(dst, src, srcOff, shift, shiftType));
  }
}

RelocatableInst::UniquePtr Subrs(CPUMode cpuMode, RegLLVM dst, RegLLVM src,
                                 RegLLVM srcOff, unsigned shift,
                                 unsigned shiftType) {
  if (cpuMode == CPUMode::ARM) {
    return NoReloc::unique(subrsi(dst, src, srcOff, shift, shiftType));
  } else {
    return NoReloc::unique(t2subrsi(dst, src, srcOff, shift, shiftType));
  }
}

RelocatableInst::UniquePtr LdmIA(CPUMode cpuMode, RegLLVM base,
                                 unsigned int regMask, bool wback) {
  QBDI_REQUIRE_ABORT(cpuMode == CPUMode::ARM, "Available only in ARM mode");

  return NoReloc::unique(ldmia(base, regMask, wback));
}

RelocatableInst::UniquePtr StmIA(CPUMode cpuMode, RegLLVM base,
                                 unsigned int regMask, bool wback) {
  QBDI_REQUIRE_ABORT(cpuMode == CPUMode::ARM, "Available only in ARM mode");

  return NoReloc::unique(stmia(base, regMask, wback));
}

RelocatableInst::UniquePtr VLdmIA(CPUMode cpuMode, RegLLVM base,
                                  unsigned int reg, unsigned int nreg,
                                  bool wback) {
  QBDI_REQUIRE_ABORT(cpuMode == CPUMode::ARM, "Available only in ARM mode");

  return NoReloc::unique(vldmia(base, llvm::ARM::D0 + reg, nreg, wback));
}

RelocatableInst::UniquePtr VStmIA(CPUMode cpuMode, RegLLVM base,
                                  unsigned int reg, unsigned int nreg,
                                  bool wback) {
  QBDI_REQUIRE_ABORT(cpuMode == CPUMode::ARM, "Available only in ARM mode");

  return NoReloc::unique(vstmia(base, llvm::ARM::D0 + reg, nreg, wback));
}

RelocatableInst::UniquePtr Mrs(CPUMode cpuMode, Reg reg) {
  if (cpuMode == CPUMode::ARM) {
    return NoReloc::unique(mrs(reg));
  } else {
    return NoReloc::unique(t2mrs(reg));
  }
}

RelocatableInst::UniquePtr Msr(CPUMode cpuMode, Reg reg) {
  if (cpuMode == CPUMode::ARM) {
    return NoReloc::unique(msr(reg));
  } else {
    return NoReloc::unique(t2msr(reg));
  }
}

RelocatableInst::UniquePtr Vmrs(CPUMode cpuMode, Reg reg) {
  QBDI_REQUIRE_ABORT(cpuMode == CPUMode::ARM, "Available only in ARM mode");

  return NoReloc::unique(vmrs(reg));
}

RelocatableInst::UniquePtr Vmsr(CPUMode cpuMode, Reg reg) {
  QBDI_REQUIRE_ABORT(cpuMode == CPUMode::ARM, "Available only in ARM mode");

  return NoReloc::unique(vmsr(reg));
}

RelocatableInst::UniquePtr Bkpt(CPUMode cpuMode, unsigned int value) {
  QBDI_REQUIRE_ABORT(cpuMode == CPUMode::ARM, "Available only in ARM mode");

  return NoReloc::unique(bkpt(value));
}

RelocatableInst::UniquePtr T2it(CPUMode cpuMode, unsigned int cond,
                                unsigned pred) {
  QBDI_REQUIRE_ABORT(cpuMode == CPUMode::Thumb, "Available only in Thumb mode");

  return NoReloc::unique(t2it(cond, pred));
}

RelocatableInst::UniquePtrVec Addc(CPUMode cpuMode, RegLLVM dst, RegLLVM src,
                                   Constant val, RegLLVM temp) {
  return Addc(cpuMode, dst, src, val, temp, llvm::ARMCC::AL);
}

RelocatableInst::UniquePtrVec Addc(CPUMode cpuMode, RegLLVM dst, RegLLVM src,
                                   Constant val, RegLLVM temp,
                                   unsigned int cond) {

  sword sval = static_cast<sword>(val);
  bool isNeg = (sval < 0);
  rword absVal = isNeg ? -sval : sval;

  if (cpuMode == CPUMode::ARM) {
    QBDI_REQUIRE_ABORT(temp != src,
                       "Source register cannot be used as Temp register");

    // if the value is already compatible, used it without change
    if (armExpandCompatible(absVal)) {
      if (isNeg) {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(sub(dst, src, absVal, cond)));
      } else {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(add(dst, src, absVal, cond)));
      }
    } else if ((absVal & 0xffff) == absVal and
               armExpandCompatible(absVal & 0xff) and
               armExpandCompatible(absVal & 0xff00)) {
      if (isNeg) {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(sub(temp, src, absVal & 0xff, cond)),
            NoReloc::unique(sub(dst, temp, absVal & 0xff00, cond)));
      } else {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(add(temp, src, absVal & 0xff, cond)),
            NoReloc::unique(add(dst, temp, absVal & 0xff00, cond)));
      }
    } else {
      if (isNeg) {
        return conv_unique<RelocatableInst>(
            LoadImmCC::unique(temp, absVal, cond),
            NoReloc::unique(subr(dst, src, temp, cond)));
      } else {
        return conv_unique<RelocatableInst>(
            LoadImmCC::unique(temp, absVal, cond),
            NoReloc::unique(addr(dst, src, temp, cond)));
      }
    }
  } else {
    RelocatableInst::UniquePtrVec vec;

    // if the value is already compatible, used it without change
    if (absVal < 4096) {
      if (cond != llvm::ARMCC::AL) {
        vec.push_back(
            T2it(cpuMode, cond, (unsigned)llvm::ARM::PredBlockMask::T));
      }
      if (isNeg) {
        vec.push_back(NoReloc::unique(t2sub(dst, src, absVal, cond)));
      } else {
        vec.push_back(NoReloc::unique(t2add(dst, src, absVal, cond)));
      }
    } else {
      if (cond != llvm::ARMCC::AL) {
        vec.push_back(
            T2it(cpuMode, cond, (unsigned)llvm::ARM::PredBlockMask::TT));
      }
      vec.push_back(LoadImmCC::unique(temp, absVal, cond));
      if (isNeg) {
        vec.push_back(NoReloc::unique(t2subr(dst, src, temp, cond)));
      } else {
        vec.push_back(NoReloc::unique(t2addr(dst, src, temp, cond)));
      }
    }
    return vec;
  }
}

RelocatableInst::UniquePtr Cmp(CPUMode cpuMode, RegLLVM src, rword imm) {
  if (cpuMode == CPUMode::ARM) {
    return NoReloc::unique(cmp(src, imm));
  } else {
    return NoReloc::unique(t2cmp(src, imm));
  }
}

RelocatableInst::UniquePtr Branch(CPUMode cpuMode, sword offset,
                                  bool addBranchLen) {
  return BranchCC(cpuMode, offset, llvm::ARMCC::AL, false, addBranchLen);
}

RelocatableInst::UniquePtr BranchCC(CPUMode cpuMode, sword offset,
                                    unsigned cond, bool withinITBlock,
                                    bool addBranchLen) {

  QBDI_DEBUG("BranchCC {} offset={} cond={} addBranchLen={}", cpuMode, offset,
             cond, addBranchLen);
  if (cpuMode == CPUMode::ARM) {
    if (addBranchLen and offset > 0) {
      offset += 4;
    }
    return NoReloc::unique(bcc(offset - 8, cond));
  } else {
    if (addBranchLen and offset > 0) {
      offset += 2;
    }
    if (cond == llvm::ARMCC::AL or withinITBlock) {
      if (-2048 < (offset - 4) and (offset - 4) < 2048) {
        return NoReloc::unique(tbranchIT(offset - 4, cond));
      } else {
        if (addBranchLen and offset > 0) {
          offset += 2;
        }
        return NoReloc::unique(t2branchIT(offset - 4, cond));
      }
    } else {
      if (-256 < (offset - 4) and (offset - 4) < 256) {
        return NoReloc::unique(tbcc(offset - 4, cond));
      } else {
        if (addBranchLen and offset > 0) {
          offset += 2;
        }
        return NoReloc::unique(t2bcc(offset - 4, cond));
      }
    }
  }
}

} // namespace QBDI
