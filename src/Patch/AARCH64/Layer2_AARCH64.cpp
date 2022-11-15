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
#include "AArch64InstrInfo.h"
#include "MCTargetDesc/AArch64AddressingModes.h"
#include "Utils/AArch64BaseInfo.h"

#include "llvm/MC/MCInst.h"

#include "Patch/AARCH64/Layer2_AARCH64.h"
#include "Patch/AARCH64/RelocatableInst_AARCH64.h"
#include "Utility/LogSys.h"

namespace QBDI {

llvm::MCInst st1_post_inc(RegLLVM regs, RegLLVM base) {
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::ST1Fourv2d_POST);
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(regs.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(llvm::AArch64::XZR));
  return inst;
}

llvm::MCInst ld1_post_inc(RegLLVM regs, RegLLVM base) {
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::LD1Fourv2d_POST);
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(regs.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(llvm::AArch64::XZR));
  return inst;
}

llvm::MCInst addr(RegLLVM dst, RegLLVM src) { return addr(dst, dst, src); }

llvm::MCInst addr(RegLLVM dst, RegLLVM src1, RegLLVM src2) {
  QBDI_REQUIRE_ABORT(src2 != llvm::AArch64::SP,
                     "SP cannot be the second operand");
  llvm::MCInst inst;
  if (dst != llvm::AArch64::SP and src1 != llvm::AArch64::SP) {
    inst.setOpcode(llvm::AArch64::ADDXrs);
    inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
    inst.addOperand(llvm::MCOperand::createReg(src1.getValue()));
    inst.addOperand(llvm::MCOperand::createReg(src2.getValue()));
    inst.addOperand(llvm::MCOperand::createImm(0));
  } else {
    inst.setOpcode(llvm::AArch64::ADDXrx64);
    inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
    inst.addOperand(llvm::MCOperand::createReg(src1.getValue()));
    inst.addOperand(llvm::MCOperand::createReg(src2.getValue()));
    inst.addOperand(llvm::MCOperand::createImm(llvm::AArch64_AM::UXTX << 3));
  }
  return inst;
}

llvm::MCInst addr(RegLLVM dst, RegLLVM src1, RegLLVM src2, ShiftExtendType type,
                  unsigned int shift) {

  QBDI_REQUIRE_ABORT(shift <= 4, "Unsupported shift {}", shift);
  unsigned immValue;
  switch (type) {
    case UXTB:
      immValue = (llvm::AArch64_AM::UXTB << 3) | (shift & 0x7);
      break;
    case UXTH:
      immValue = (llvm::AArch64_AM::UXTH << 3) | (shift & 0x7);
      break;
    case UXTW:
      immValue = (llvm::AArch64_AM::UXTW << 3) | (shift & 0x7);
      break;
    case UXTX:
      immValue = (llvm::AArch64_AM::UXTX << 3) | (shift & 0x7);
      break;
    case SXTB:
      immValue = (llvm::AArch64_AM::SXTB << 3) | (shift & 0x7);
      break;
    case SXTH:
      immValue = (llvm::AArch64_AM::SXTH << 3) | (shift & 0x7);
      break;
    case SXTW:
      immValue = (llvm::AArch64_AM::SXTW << 3) | (shift & 0x7);
      break;
    case SXTX:
      immValue = (llvm::AArch64_AM::SXTX << 3) | (shift & 0x7);
      break;
    default:
      QBDI_ABORT("Unexpected type %d", type);
  }

  llvm::MCInst inst;
  RegLLVM extReg;
  if (type == UXTX || type == SXTX) {
    extReg = llvm::getXRegFromWReg(src2.getValue());
    inst.setOpcode(llvm::AArch64::ADDXrx64);
  } else {
    extReg = llvm::getWRegFromXReg(src2.getValue());
    inst.setOpcode(llvm::AArch64::ADDXrx);
  }
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src1.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(extReg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(immValue));
  return inst;
}

llvm::MCInst addri(RegLLVM dst, RegLLVM src, rword offset) {
  int shift = 0;
  if (offset != 0 && (offset % 4096) == 0) {
    shift = 12;
    offset = offset >> 12;
    QBDI_REQUIRE_ABORT(offset < 4096, "Must be a lower than 2**24 : {}",
                       offset);
  } else {
    QBDI_REQUIRE_ABORT(offset < 4096, "Must be a lower than 4096 : {}", offset);
  }
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::ADDXri);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(shift));
  return inst;
}

llvm::MCInst subr(RegLLVM dst, RegLLVM src) { return subr(dst, dst, src); }

llvm::MCInst subr(RegLLVM dst, RegLLVM src1, RegLLVM src2) {
  llvm::MCInst inst;
  QBDI_REQUIRE_ABORT(src2 != llvm::AArch64::SP,
                     "SP cannot be the second operand");
  if (dst != llvm::AArch64::SP and src1 != llvm::AArch64::SP) {

    inst.setOpcode(llvm::AArch64::SUBXrs);
    inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
    inst.addOperand(llvm::MCOperand::createReg(src1.getValue()));
    inst.addOperand(llvm::MCOperand::createReg(src2.getValue()));
    inst.addOperand(llvm::MCOperand::createImm(0));
  } else {

    inst.setOpcode(llvm::AArch64::SUBXrx64);
    inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
    inst.addOperand(llvm::MCOperand::createReg(src1.getValue()));
    inst.addOperand(llvm::MCOperand::createReg(src2.getValue()));
    inst.addOperand(llvm::MCOperand::createImm(llvm::AArch64_AM::UXTX << 3));
  }
  return inst;
}

llvm::MCInst subri(RegLLVM dst, RegLLVM src, rword offset) {
  int shift = 0;
  if (offset != 0 && (offset % 4096) == 0) {
    shift = 12;
    offset = offset >> 12;
    QBDI_REQUIRE_ABORT(offset < 4096, "Must be a lower than 2**24 : {}",
                       offset);
  } else {
    QBDI_REQUIRE_ABORT(offset < 4096, "Must be a lower than 4096 : {}", offset);
  }
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::SUBXri);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  inst.addOperand(llvm::MCOperand::createImm(shift));
  return inst;
}

llvm::MCInst br(RegLLVM reg) {
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::BR);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  return inst;
}

llvm::MCInst blr(RegLLVM reg) {
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::BLR);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  return inst;
}

llvm::MCInst branch(rword offset) {
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::B);
  inst.addOperand(llvm::MCOperand::createImm(offset / 4));
  return inst;
}

llvm::MCInst cbz(RegLLVM reg, sword offset) {
  QBDI_REQUIRE_ABORT(offset % 4 == 0,
                     "offset = SignExtend(imm19:'00', 64); (current : {})",
                     offset);
  QBDI_REQUIRE_ABORT(-(1 << 20) <= offset and offset < (1 << 20),
                     "offset = SignExtend(imm19:'00', 64); (current : {})",
                     offset);
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::CBZX);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset / 4));
  return inst;
}

llvm::MCInst ret(RegLLVM reg) {
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::RET);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  return inst;
}

llvm::MCInst adr(RegLLVM reg, sword offset) {
  QBDI_REQUIRE_ABORT(-(1 << 20) <= offset and offset < (1 << 20),
                     "offset = SignExtend(imm21, 64); (current : {})", offset);
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::ADR);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  return inst;
}

llvm::MCInst adrp(RegLLVM reg, sword offset) {
  QBDI_REQUIRE_ABORT(offset % (1 << 12) == 0,
                     "offset = SignExtend(imm21:Zeros(12)); (current : {})",
                     offset);
  QBDI_REQUIRE_ABORT(-(1ll << 32) <= offset and offset < (1ll << 32),
                     "offset = SignExtend(imm21:Zeros(12)); (current : {})",
                     offset);
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::ADRP);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset / 0x1000));
  return inst;
}

llvm::MCInst nop() {
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::HINT);
  inst.addOperand(llvm::MCOperand::createImm(0));
  return inst;
}

llvm::MCInst ldr(RegLLVM dest, RegLLVM base, rword offset) {
  sword soffset = static_cast<sword>(offset);
  if (offset % 8 == 0 and soffset >= 0) {
    return ldrui(dest, base, offset / 8);
  }
  return ldri(dest, base, soffset);
}

llvm::MCInst ldri(RegLLVM dest, RegLLVM base, sword offset) {

  QBDI_REQUIRE_ABORT(-(1 << 8) <= offset and offset < (1 << 8),
                     "offset = SignExtend(imm9, 64); (current : {})", offset);

  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::LDURXi);
  inst.addOperand(llvm::MCOperand::createReg(dest.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  return inst;
}

llvm::MCInst ldrui(RegLLVM dest, RegLLVM base, rword offset) {

  QBDI_REQUIRE_ABORT(
      offset < (1 << 12),
      "offset = LSL(ZeroExtend(imm12, 64), scale); (current : {})", offset);

  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::LDRXui);
  inst.addOperand(llvm::MCOperand::createReg(dest.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  return inst;
}

llvm::MCInst ldrw(RegLLVM dest, RegLLVM base, rword offset) {
  sword soffset = static_cast<sword>(offset);
  if (offset % 4 == 0 and soffset >= 0) {
    return ldrwui(dest, base, offset / 4);
  }
  return ldrwi(dest, base, soffset);
}

llvm::MCInst ldrwi(RegLLVM dest, RegLLVM base, sword offset) {

  QBDI_REQUIRE_ABORT(-(1 << 8) <= offset and offset < (1 << 8),
                     "offset = SignExtend(imm9, 64); (current : {})", offset);

  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::LDURWi);
  inst.addOperand(llvm::MCOperand::createReg(dest.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  return inst;
}

llvm::MCInst ldrwui(RegLLVM dest, RegLLVM base, rword offset) {

  QBDI_REQUIRE_ABORT(
      offset < (1 << 12),
      "offset = LSL(ZeroExtend(imm12, 64), scale); (current : {})", offset);

  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::LDRWui);
  inst.addOperand(llvm::MCOperand::createReg(dest.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  return inst;
}

llvm::MCInst ldrh(RegLLVM dest, RegLLVM base, rword offset) {
  sword soffset = static_cast<sword>(offset);
  if (offset % 2 == 0 and soffset >= 0) {
    return ldrhui(dest, base, offset / 2);
  }
  return ldrhi(dest, base, soffset);
}

llvm::MCInst ldrhi(RegLLVM dest, RegLLVM base, sword offset) {

  QBDI_REQUIRE_ABORT(-(1 << 8) <= offset and offset < (1 << 8),
                     "offset = SignExtend(imm9, 64); (current : {})", offset);

  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::LDURHHi);
  inst.addOperand(llvm::MCOperand::createReg(dest.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  return inst;
}

llvm::MCInst ldrhui(RegLLVM dest, RegLLVM base, rword offset) {

  QBDI_REQUIRE_ABORT(offset < (1 << 12),
                     "offset = LSL(ZeroExtend(imm12, 64), 1); (current : {})",
                     offset);

  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::LDRHHui);
  inst.addOperand(llvm::MCOperand::createReg(dest.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  return inst;
}

llvm::MCInst ldrb(RegLLVM dest, RegLLVM base, rword offset) {

  QBDI_REQUIRE_ABORT(offset < (1 << 12),
                     "offset = SignExtend(imm9, 64); (current : {})", offset);

  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::LDRBBui);
  inst.addOperand(llvm::MCOperand::createReg(dest.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  return inst;
}

llvm::MCInst ldxrb(RegLLVM dest, RegLLVM addr) {
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::LDXRB);
  inst.addOperand(llvm::MCOperand::createReg(dest.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(addr.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(0));
  return inst;
}

llvm::MCInst ldp(RegLLVM dest1, RegLLVM dest2, RegLLVM base, sword offset) {
  QBDI_REQUIRE_ABORT(offset % 8 == 0, "Must be a multiple of 8; (current : {})",
                     offset);
  QBDI_REQUIRE_ABORT(-512 <= offset and offset <= 504,
                     "Must be in the range [-512, 504]; (current : {})",
                     offset);

  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::LDPXi);
  inst.addOperand(llvm::MCOperand::createReg(dest1.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(dest2.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset / 8));
  return inst;
}

llvm::MCInst ldr_post_inc(RegLLVM dest, RegLLVM base, sword imm) {
  QBDI_REQUIRE_ABORT(-256 <= imm and imm <= 255,
                     "Must be in the range [-512, 504]; (current : {})", imm);

  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::LDRXpost);
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(dest.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(imm));
  return inst;
}

llvm::MCInst ldp_post_inc(RegLLVM dest1, RegLLVM dest2, RegLLVM base,
                          sword imm) {
  QBDI_REQUIRE_ABORT(imm % 8 == 0, "Must be a multiple of 8; (current : {})",
                     imm);
  QBDI_REQUIRE_ABORT(-512 <= imm and imm <= 504,
                     "Must be in the range [-512, 504]; (current : {})", imm);

  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::LDPXpost);
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(dest1.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(dest2.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(imm / 8));
  return inst;
}

llvm::MCInst str(RegLLVM src, RegLLVM base, rword offset) {
  sword soffset = static_cast<sword>(offset);
  if (offset % 8 == 0 and soffset >= 0) {
    return strui(src, base, offset / 8);
  }
  return stri(src, base, soffset);
}

llvm::MCInst stri(RegLLVM src, RegLLVM base, sword offset) {
  QBDI_REQUIRE_ABORT(-(1 << 8) <= offset and offset < (1 << 8),
                     "offset = SignExtend(imm9, 64); (current : {})", offset);
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::STURXi);
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  return inst;
}

llvm::MCInst strui(RegLLVM src, RegLLVM base, rword offset) {
  QBDI_REQUIRE_ABORT(
      offset < (1 << 12),
      "offset = LSL(ZeroExtend(imm12, 64), scale); (current : {})", offset);
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::STRXui);
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset));
  return inst;
}

llvm::MCInst stp(RegLLVM src1, RegLLVM src2, RegLLVM base, sword offset) {
  QBDI_REQUIRE_ABORT(offset % 8 == 0, "Must be a multiple of 8; (current : {})",
                     offset);
  QBDI_REQUIRE_ABORT(-512 <= offset and offset <= 504,
                     "Must be in the range [-512, 504]; (current : {})",
                     offset);

  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::STPXi);
  inst.addOperand(llvm::MCOperand::createReg(src1.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src2.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(offset / 8));
  return inst;
}

llvm::MCInst str_pre_inc(RegLLVM reg, RegLLVM base, sword imm) {
  QBDI_REQUIRE_ABORT(-256 <= imm and imm <= 255,
                     "Must be in the range [-512, 504]; (current : {})", imm);

  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::STRXpre);
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(base.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(imm));
  return inst;
}

llvm::MCInst lsl(RegLLVM dst, RegLLVM src, size_t shift) {
  QBDI_REQUIRE_ABORT(shift < 64, "max shift of 64; (current : {})", shift);

  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::UBFMXri);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));

  int64_t imms = 63 - shift;
  int64_t immr = (imms + 1) % 64;

  inst.addOperand(llvm::MCOperand::createImm(immr));
  inst.addOperand(llvm::MCOperand::createImm(imms));
  return inst;
}

llvm::MCInst lsr(RegLLVM dst, RegLLVM src, size_t shift) {
  QBDI_REQUIRE_ABORT(shift < 64, "max shift of 64; (current : {})", shift);

  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::UBFMXri);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(shift));
  inst.addOperand(llvm::MCOperand::createImm(63));
  return inst;
}

llvm::MCInst msr(unsigned sysdst, RegLLVM src) {
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::MSR);
  inst.addOperand(llvm::MCOperand::createImm(sysdst));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  return inst;
}

llvm::MCInst mrs(RegLLVM dst, unsigned syssrc) {
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::MRS);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(syssrc));
  return inst;
}

llvm::MCInst movrr(RegLLVM dst, RegLLVM src) {
  if (dst == llvm::AArch64::SP or src == llvm::AArch64::SP) {
    return addri(dst, src, 0);
  }
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::ORRXrs);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(llvm::AArch64::XZR));
  inst.addOperand(llvm::MCOperand::createReg(src.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(0));
  return inst;
}

llvm::MCInst movri(RegLLVM dst, uint16_t v) {
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::MOVZXi);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(v));
  inst.addOperand(llvm::MCOperand::createImm(0));
  return inst;
}

llvm::MCInst orrrs(RegLLVM dst, RegLLVM src1, RegLLVM src2, unsigned lshift) {
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::ORRXrs);
  inst.addOperand(llvm::MCOperand::createReg(dst.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src1.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(src2.getValue()));
  inst.addOperand(llvm::MCOperand::createImm(lshift));
  return inst;
}

llvm::MCInst brk(unsigned imm) {
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::BRK);
  inst.addOperand(llvm::MCOperand::createImm(imm));
  return inst;
}

llvm::MCInst hint(unsigned imm) {
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::HINT);
  inst.addOperand(llvm::MCOperand::createImm(imm));
  return inst;
}

llvm::MCInst xpacd(RegLLVM reg) {
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::XPACD);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  return inst;
}

llvm::MCInst xpaci(RegLLVM reg) {
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::XPACI);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  return inst;
}

llvm::MCInst autia(RegLLVM reg, RegLLVM ctx) {
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::AUTIA);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(ctx.getValue()));
  return inst;
}

llvm::MCInst autib(RegLLVM reg, RegLLVM ctx) {
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::AUTIB);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  inst.addOperand(llvm::MCOperand::createReg(ctx.getValue()));
  return inst;
}

llvm::MCInst autiza(RegLLVM reg) {
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::AUTIZA);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  return inst;
}

llvm::MCInst autizb(RegLLVM reg) {
  llvm::MCInst inst;
  inst.setOpcode(llvm::AArch64::AUTIZB);
  inst.addOperand(llvm::MCOperand::createReg(reg.getValue()));
  return inst;
}

// =================================================

RelocatableInst::UniquePtr Ld1PostInc(RegLLVM regs, RegLLVM base) {
  return NoReloc::unique(ld1_post_inc(regs, base));
}

RelocatableInst::UniquePtr St1PostInc(RegLLVM regs, RegLLVM base) {
  return NoReloc::unique(st1_post_inc(regs, base));
}

RelocatableInst::UniquePtr Add(RegLLVM dst, RegLLVM src, Constant val) {
  sword sval = static_cast<sword>(val);
  if (sval < 0) {
    return NoReloc::unique(subri(dst, src, -sval));
  }
  return NoReloc::unique(addri(dst, src, val));
}

RelocatableInst::UniquePtrVec Addc(RegLLVM dst, RegLLVM src, Constant val,
                                   RegLLVM temp) {
  QBDI_REQUIRE_ABORT(temp != src,
                     "Cannot add an 16 bits constant without 2 registers");
  sword sval = static_cast<sword>(val);
  // if the value can be used in a add or sub, do it
  if ((-4096 < sval) and (sval < 4096)) {
    return conv_unique<RelocatableInst>(Add(dst, src, val));
  }
  if (sval < 0) {
    return conv_unique<RelocatableInst>(LoadImm::unique(temp, -sval),
                                        NoReloc::unique(subr(dst, src, temp)));
  } else {
    return conv_unique<RelocatableInst>(LoadImm::unique(temp, sval),
                                        NoReloc::unique(addr(dst, src, temp)));
  }
}

RelocatableInst::UniquePtr Addr(RegLLVM dst, RegLLVM src) {
  return NoReloc::unique(addr(dst, src));
}

RelocatableInst::UniquePtr Addr(RegLLVM dst, RegLLVM src1, RegLLVM src2,
                                ShiftExtendType type, Constant shift) {
  return NoReloc::unique(addr(dst, src1, src2, type, shift));
}

RelocatableInst::UniquePtr Br(RegLLVM reg) { return NoReloc::unique(br(reg)); }

RelocatableInst::UniquePtr Blr(RegLLVM reg) {
  return NoReloc::unique(blr(reg));
}

RelocatableInst::UniquePtr Cbz(RegLLVM reg, Constant offset) {
  return NoReloc::unique(cbz(reg, offset));
}

RelocatableInst::UniquePtr Ret() {
  return NoReloc::unique(ret(llvm::AArch64::LR));
}

RelocatableInst::UniquePtr Adr(RegLLVM reg, rword offset) {
  return NoReloc::unique(adr(reg, offset));
}

RelocatableInst::UniquePtr Adrp(RegLLVM reg, rword offset) {
  QBDI_REQUIRE_ABORT(offset % 0x1000 == 0,
                     "Must be a multiple of 0x1000; (current : 0x{:x})",
                     offset);
  return NoReloc::unique(adrp(reg, offset));
}

RelocatableInst::UniquePtr Nop() { return NoReloc::unique(nop()); }

RelocatableInst::UniquePtr Ldr(RegLLVM reg, RegLLVM base, rword offset) {
  return NoReloc::unique(ldr(reg, base, offset));
}

RelocatableInst::UniquePtr Ldrw(RegLLVM reg, RegLLVM base, rword offset) {
  QBDI_REQUIRE(llvm::AArch64::X0 <= reg.getValue() and
               reg.getValue() <= llvm::AArch64::X28);
  // need a w register
  RegLLVM wreg = llvm::getWRegFromXReg(reg.getValue());
  return NoReloc::unique(ldrw(wreg, base, offset));
}

RelocatableInst::UniquePtr Ldrh(RegLLVM reg, RegLLVM base, rword offset) {
  QBDI_REQUIRE(llvm::AArch64::X0 <= reg.getValue() and
               reg.getValue() <= llvm::AArch64::X28);
  // need a w register
  RegLLVM wreg = llvm::getWRegFromXReg(reg.getValue());
  return NoReloc::unique(ldrh(wreg, base, offset));
}

RelocatableInst::UniquePtr Ldrb(RegLLVM reg, RegLLVM base, rword offset) {
  QBDI_REQUIRE(llvm::AArch64::X0 <= reg.getValue() and
               reg.getValue() <= llvm::AArch64::X28);
  // need a w register
  RegLLVM wreg = llvm::getWRegFromXReg(reg.getValue());
  return NoReloc::unique(ldrb(wreg, base, offset));
}

RelocatableInst::UniquePtr Ldr(RegLLVM reg, Offset offset) {
  return LoadDataBlock::unique(reg, offset);
}

RelocatableInst::UniquePtr Ldxrb(RegLLVM dst, RegLLVM addr) {
  QBDI_REQUIRE(llvm::AArch64::X0 <= dst.getValue() and
               dst.getValue() <= llvm::AArch64::X28);
  // need a w register
  RegLLVM wreg = llvm::getWRegFromXReg(dst.getValue());
  return NoReloc::unique(ldxrb(wreg, addr));
}

RelocatableInst::UniquePtr LdrPost(RegLLVM dest, RegLLVM base, Constant imm) {
  return NoReloc::unique(ldr_post_inc(dest, base, imm));
}

RelocatableInst::UniquePtr Ldp(RegLLVM dest1, RegLLVM dest2, RegLLVM base,
                               Offset offset) {
  return NoReloc::unique(ldp(dest1, dest2, base, offset));
}

RelocatableInst::UniquePtr LdpPost(RegLLVM dest1, RegLLVM dest2, RegLLVM base,
                                   Constant imm) {
  return NoReloc::unique(ldp_post_inc(dest1, dest2, base, imm));
}

RelocatableInst::UniquePtr Str(RegLLVM reg, RegLLVM base, Offset offset) {
  return NoReloc::unique(str(reg, base, offset));
}

RelocatableInst::UniquePtr Str(RegLLVM reg, Offset offset) {
  return StoreDataBlock::unique(reg, offset);
}

RelocatableInst::UniquePtr StrPre(RegLLVM src, RegLLVM base, Constant imm) {
  return NoReloc::unique(str_pre_inc(src, base, imm));
}

RelocatableInst::UniquePtr Stp(RegLLVM src1, RegLLVM src2, RegLLVM base,
                               Offset offset) {
  return NoReloc::unique(stp(src1, src2, base, offset));
}

RelocatableInst::UniquePtr Lsl(RegLLVM dst, RegLLVM src, Constant shift) {
  return NoReloc::unique(lsl(dst, src, shift));
}

RelocatableInst::UniquePtr Lsr(RegLLVM dst, RegLLVM src, Constant shift) {
  return NoReloc::unique(lsr(dst, src, shift));
}

RelocatableInst::UniquePtr ReadTPIDR(RegLLVM reg) {
  return NoReloc::unique(mrs(reg, llvm::AArch64SysReg::TPIDR_EL0));
}

RelocatableInst::UniquePtr WriteTPIDR(RegLLVM reg) {
  return NoReloc::unique(msr(llvm::AArch64SysReg::TPIDR_EL0, reg));
}

RelocatableInst::UniquePtr WriteSRinTPIDR() {
  return SetScratchRegister::unique(msr(llvm::AArch64SysReg::TPIDR_EL0, 0), 1);
}

RelocatableInst::UniquePtr ReadNZCV(RegLLVM reg) {
  return NoReloc::unique(mrs(reg, llvm::AArch64SysReg::NZCV));
}

RelocatableInst::UniquePtr WriteNZCV(RegLLVM reg) {
  return NoReloc::unique(msr(llvm::AArch64SysReg::NZCV, reg));
}

RelocatableInst::UniquePtr ReadFPCR(RegLLVM reg) {
  return NoReloc::unique(mrs(reg, llvm::AArch64SysReg::FPCR));
}

RelocatableInst::UniquePtr WriteFPCR(RegLLVM reg) {
  return NoReloc::unique(msr(llvm::AArch64SysReg::FPCR, reg));
}

RelocatableInst::UniquePtr ReadFPSR(RegLLVM reg) {
  return NoReloc::unique(mrs(reg, llvm::AArch64SysReg::FPSR));
}

RelocatableInst::UniquePtr WriteFPSR(RegLLVM reg) {
  return NoReloc::unique(msr(llvm::AArch64SysReg::FPSR, reg));
}

RelocatableInst::UniquePtr Mov(RegLLVM dst, RegLLVM src) {
  return MovReg::unique(dst, src);
}

RelocatableInst::UniquePtr Mov(RegLLVM dst, Constant constant) {
  return LoadImm::unique(dst, constant);
}

RelocatableInst::UniquePtr Orrs(RegLLVM dst, RegLLVM src1, RegLLVM src2,
                                Constant lshift) {
  return NoReloc::unique(orrrs(dst, src1, src2, lshift));
}

RelocatableInst::UniquePtr BreakPoint() { return NoReloc::unique(brk(0)); }

RelocatableInst::UniquePtr BTIc() { return NoReloc::unique(hint(0x22)); }

RelocatableInst::UniquePtr BTIj() { return NoReloc::unique(hint(0x24)); }

RelocatableInst::UniquePtr Xpacd(RegLLVM reg) {
  return NoReloc::unique(xpacd(reg));
}

RelocatableInst::UniquePtr Xpaci(RegLLVM reg) {
  return NoReloc::unique(xpaci(reg));
}

RelocatableInst::UniquePtr Autia(RegLLVM reg, RegLLVM ctx) {
  return NoReloc::unique(autia(reg, ctx));
}

RelocatableInst::UniquePtr Autib(RegLLVM reg, RegLLVM ctx) {
  return NoReloc::unique(autib(reg, ctx));
}

RelocatableInst::UniquePtr Autiza(RegLLVM reg) {
  return NoReloc::unique(autiza(reg));
}

RelocatableInst::UniquePtr Autizb(RegLLVM reg) {
  return NoReloc::unique(autizb(reg));
}

} // namespace QBDI
