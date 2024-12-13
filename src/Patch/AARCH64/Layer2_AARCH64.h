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
#ifndef LAYER2_AARCH64_H
#define LAYER2_AARCH64_H

#include <memory>
#include <stdint.h>
#include <vector>

#include "llvm/MC/MCInst.h"

#include "QBDI/State.h"
#include "Patch/Types.h"

namespace QBDI {
class RelocatableInst;

// proxy to llvm::AArch64_AM::ShiftExtendType
// see MCTargetDesc/AArch64AddressingModes.h
enum ShiftExtendType {
  UXTB,
  UXTH,
  UXTW,
  UXTX,
  SXTB,
  SXTH,
  SXTW,
  SXTX,
};
unsigned format_as(ShiftExtendType type);

// low level layer 2

llvm::MCInst ld1_post_inc(RegLLVM regs, RegLLVM base);
llvm::MCInst st1_post_inc(RegLLVM regs, RegLLVM base);

llvm::MCInst addr(RegLLVM dst, RegLLVM src);
llvm::MCInst addr(RegLLVM dst, RegLLVM src1, RegLLVM src2);
llvm::MCInst addr(RegLLVM dst, RegLLVM src1, RegLLVM src2, ShiftExtendType type,
                  unsigned int shift);
llvm::MCInst addri(RegLLVM dst, RegLLVM src, rword offset);

llvm::MCInst subr(RegLLVM dst, RegLLVM src);
llvm::MCInst subr(RegLLVM dst, RegLLVM src1, RegLLVM src2);
llvm::MCInst subri(RegLLVM dst, RegLLVM src, rword offset);

llvm::MCInst br(RegLLVM reg);
llvm::MCInst blr(RegLLVM reg);
llvm::MCInst branch(rword offset);
llvm::MCInst cbz(RegLLVM reg, sword offset);
llvm::MCInst ret(RegLLVM reg);
llvm::MCInst adr(RegLLVM reg, sword offset);
llvm::MCInst adrp(RegLLVM reg, sword offset = 0);
llvm::MCInst nop();

llvm::MCInst ldr(RegLLVM dest, RegLLVM base, rword offset);
llvm::MCInst ldri(RegLLVM dest, RegLLVM base, sword offset);
llvm::MCInst ldrui(RegLLVM dest, RegLLVM base, rword offset);
llvm::MCInst ldrw(RegLLVM dest, RegLLVM base, rword offset);
llvm::MCInst ldrwi(RegLLVM dest, RegLLVM base, sword offset);
llvm::MCInst ldrwui(RegLLVM dest, RegLLVM base, rword offset);
llvm::MCInst ldrh(RegLLVM dest, RegLLVM base, rword offset);
llvm::MCInst ldrhi(RegLLVM dest, RegLLVM base, sword offset);
llvm::MCInst ldrhui(RegLLVM dest, RegLLVM base, rword offset);
llvm::MCInst ldrb(RegLLVM dest, RegLLVM base, rword offset);
llvm::MCInst ldxrb(RegLLVM dest, RegLLVM addr);
llvm::MCInst ldp(RegLLVM dest1, RegLLVM dest2, RegLLVM base, sword offset);
llvm::MCInst ldr_post_inc(RegLLVM dest, RegLLVM base, sword imm);
llvm::MCInst ldp_post_inc(RegLLVM dest1, RegLLVM dest2, RegLLVM base,
                          sword imm);

llvm::MCInst str(RegLLVM src, RegLLVM base, rword offset);
llvm::MCInst stri(RegLLVM src, RegLLVM base, sword offset);
llvm::MCInst strui(RegLLVM src, RegLLVM base, rword offset);
llvm::MCInst stp(RegLLVM src1, RegLLVM src2, RegLLVM base, sword offset);
llvm::MCInst str_pre_inc(RegLLVM reg, RegLLVM base, sword imm);

llvm::MCInst lsl(RegLLVM dst, RegLLVM src, size_t shift);
llvm::MCInst lsr(RegLLVM dst, RegLLVM src, size_t shift);

llvm::MCInst msr(unsigned sysdst, RegLLVM src);
llvm::MCInst mrs(RegLLVM dst, unsigned syssrc);

llvm::MCInst movrr(RegLLVM dst, RegLLVM src);
llvm::MCInst movri(RegLLVM dst, uint16_t v);
llvm::MCInst orrrs(RegLLVM dst, RegLLVM src1, RegLLVM src2, unsigned lshift);

llvm::MCInst brk(unsigned imm);
llvm::MCInst hint(unsigned imm);

llvm::MCInst xpacd(RegLLVM reg);
llvm::MCInst xpaci(RegLLVM reg);

llvm::MCInst autia(RegLLVM reg, RegLLVM ctx);
llvm::MCInst autib(RegLLVM reg, RegLLVM ctx);
llvm::MCInst autiza(RegLLVM reg);
llvm::MCInst autizb(RegLLVM reg);

// high level layer 2

std::unique_ptr<RelocatableInst> Ld1PostInc(RegLLVM regs, RegLLVM base);
std::unique_ptr<RelocatableInst> St1PostInc(RegLLVM regs, RegLLVM base);

std::unique_ptr<RelocatableInst> Add(RegLLVM dst, RegLLVM src, Constant val);
std::vector<std::unique_ptr<RelocatableInst>> Addc(RegLLVM dst, RegLLVM src,
                                                   Constant val, RegLLVM temp);

std::unique_ptr<RelocatableInst> Addr(RegLLVM dst, RegLLVM src);
std::unique_ptr<RelocatableInst> Addr(RegLLVM dst, RegLLVM src1, RegLLVM src2,
                                      ShiftExtendType type, Constant shift);

std::unique_ptr<RelocatableInst> Br(RegLLVM reg);
std::unique_ptr<RelocatableInst> Blr(RegLLVM reg);
std::unique_ptr<RelocatableInst> Cbz(RegLLVM reg, Constant offset);
std::unique_ptr<RelocatableInst> Ret();
std::unique_ptr<RelocatableInst> Adr(RegLLVM reg, rword offset);
std::unique_ptr<RelocatableInst> Adrp(RegLLVM reg, rword offset);
std::unique_ptr<RelocatableInst> Nop();

std::unique_ptr<RelocatableInst> Ldr(RegLLVM reg, RegLLVM base, rword offset);
std::unique_ptr<RelocatableInst> Ldrw(RegLLVM reg, RegLLVM base, rword offset);
std::unique_ptr<RelocatableInst> Ldrh(RegLLVM reg, RegLLVM base, rword offset);
std::unique_ptr<RelocatableInst> Ldrb(RegLLVM reg, RegLLVM base, rword offset);
std::unique_ptr<RelocatableInst> Ldr(RegLLVM reg, Offset offset);
std::unique_ptr<RelocatableInst> Ldxrb(RegLLVM dest, RegLLVM addr);
std::unique_ptr<RelocatableInst> LdrPost(RegLLVM dest, RegLLVM base,
                                         Constant imm);
std::unique_ptr<RelocatableInst> Ldp(RegLLVM dest1, RegLLVM dest2, RegLLVM base,
                                     Offset offset);
std::unique_ptr<RelocatableInst> LdpPost(RegLLVM dest1, RegLLVM dest2,
                                         RegLLVM base, Constant imm);

std::unique_ptr<RelocatableInst> Str(RegLLVM reg, RegLLVM base, Offset offset);
std::unique_ptr<RelocatableInst> Str(RegLLVM reg, Offset offset);
std::unique_ptr<RelocatableInst> StrPre(RegLLVM reg, RegLLVM base,
                                        Constant imm);
std::unique_ptr<RelocatableInst> Stp(RegLLVM src1, RegLLVM src2, RegLLVM base,
                                     Offset offset);

std::unique_ptr<RelocatableInst> Lsl(RegLLVM dst, RegLLVM src, Constant shift);
std::unique_ptr<RelocatableInst> Lsr(RegLLVM dst, RegLLVM src, Constant shift);

std::unique_ptr<RelocatableInst> ReadTPIDR(RegLLVM reg);
std::unique_ptr<RelocatableInst> WriteTPIDR(RegLLVM reg);
std::unique_ptr<RelocatableInst> WriteSRinTPIDR();

std::unique_ptr<RelocatableInst> ReadNZCV(RegLLVM reg);
std::unique_ptr<RelocatableInst> WriteNZCV(RegLLVM reg);

std::unique_ptr<RelocatableInst> ReadFPCR(RegLLVM reg);
std::unique_ptr<RelocatableInst> WriteFPCR(RegLLVM reg);

std::unique_ptr<RelocatableInst> ReadFPSR(RegLLVM reg);
std::unique_ptr<RelocatableInst> WriteFPSR(RegLLVM reg);

std::unique_ptr<RelocatableInst> Mov(RegLLVM dst, RegLLVM src);
std::unique_ptr<RelocatableInst> Mov(RegLLVM dst, Constant constant);
std::unique_ptr<RelocatableInst> Orrs(RegLLVM dst, RegLLVM src1, RegLLVM src2,
                                      Constant lshift);

std::unique_ptr<RelocatableInst> BreakPoint();
std::unique_ptr<RelocatableInst> BTIc();
std::unique_ptr<RelocatableInst> BTIj();

std::unique_ptr<RelocatableInst> Xpacd(RegLLVM reg);
std::unique_ptr<RelocatableInst> Xpaci(RegLLVM reg);

std::unique_ptr<RelocatableInst> Autia(RegLLVM reg, RegLLVM ctx);
std::unique_ptr<RelocatableInst> Autib(RegLLVM reg, RegLLVM ctx);
std::unique_ptr<RelocatableInst> Autiza(RegLLVM reg);
std::unique_ptr<RelocatableInst> Autizb(RegLLVM reg);

} // namespace QBDI

#endif
