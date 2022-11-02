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
#ifndef QBDI_LAYER2_ARM_H
#define QBDI_LAYER2_ARM_H

#include "Patch/RelocatableInst.h"
#include "Patch/Types.h"

namespace QBDI {

// Low level layer 2

// utils
bool armExpandCompatible(rword imm);
bool thumbExpandCompatible(rword imm);
bool t2moviCompatible(rword imm);

// LoadInst 4
llvm::MCInst popr1(RegLLVM reg, unsigned cond);

llvm::MCInst ldri12(RegLLVM reg, RegLLVM base, sword offset);
llvm::MCInst ldri12(RegLLVM reg, RegLLVM base, sword offset, unsigned cond);
llvm::MCInst ldrPost(RegLLVM base, RegLLVM reg, sword offset);
llvm::MCInst ldrPost(RegLLVM base, RegLLVM reg, sword offset, unsigned cond);
llvm::MCInst t2ldri8(RegLLVM reg, RegLLVM base, sword offset);
llvm::MCInst t2ldri8(RegLLVM reg, RegLLVM base, sword offset, unsigned cond);
llvm::MCInst t2ldri12(RegLLVM reg, RegLLVM base, sword offset);
llvm::MCInst t2ldri12(RegLLVM reg, RegLLVM base, sword offset, unsigned cond);
llvm::MCInst t2ldrPost(RegLLVM reg, RegLLVM base, sword offset);
llvm::MCInst t2ldrPost(RegLLVM reg, RegLLVM base, sword offset, unsigned cond);
llvm::MCInst t2ldrPre(RegLLVM reg, RegLLVM base, sword offset);
llvm::MCInst t2ldrPre(RegLLVM reg, RegLLVM base, sword offset, unsigned cond);

// LoadInst 2

llvm::MCInst ldrh(RegLLVM base, RegLLVM reg, unsigned int offset);
llvm::MCInst ldrh(RegLLVM base, RegLLVM reg, unsigned int offset,
                  unsigned cond);
llvm::MCInst ldrhPost(RegLLVM base, RegLLVM reg);
llvm::MCInst ldrhPost(RegLLVM base, RegLLVM reg, unsigned cond);

llvm::MCInst t2ldrh(RegLLVM base, RegLLVM reg, unsigned int offset);
llvm::MCInst t2ldrh(RegLLVM base, RegLLVM reg, unsigned int offset,
                    unsigned cond);
llvm::MCInst t2ldrhPost(RegLLVM base, RegLLVM reg);
llvm::MCInst t2ldrhPost(RegLLVM base, RegLLVM reg, unsigned cond);

llvm::MCInst t2ldrhr(RegLLVM base, RegLLVM reg, RegLLVM offReg);
llvm::MCInst t2ldrhr(RegLLVM base, RegLLVM reg, RegLLVM offReg, unsigned cond);
llvm::MCInst t2ldrhrs(RegLLVM base, RegLLVM reg, RegLLVM offReg, unsigned lsl);
llvm::MCInst t2ldrhrs(RegLLVM base, RegLLVM reg, RegLLVM offReg, unsigned lsl,
                      unsigned cond);

// LoadInst 1

llvm::MCInst ldrb(RegLLVM base, RegLLVM reg, unsigned int offset);
llvm::MCInst ldrb(RegLLVM base, RegLLVM reg, unsigned int offset,
                  unsigned cond);
llvm::MCInst ldrbPost(RegLLVM base, RegLLVM reg);
llvm::MCInst ldrbPost(RegLLVM base, RegLLVM reg, unsigned cond);

llvm::MCInst t2ldrb(RegLLVM base, RegLLVM reg, unsigned int offset);
llvm::MCInst t2ldrb(RegLLVM base, RegLLVM reg, unsigned int offset,
                    unsigned cond);
llvm::MCInst t2ldrbPost(RegLLVM base, RegLLVM reg);
llvm::MCInst t2ldrbPost(RegLLVM base, RegLLVM reg, unsigned cond);

llvm::MCInst t2ldrbr(RegLLVM base, RegLLVM reg, RegLLVM offReg);
llvm::MCInst t2ldrbr(RegLLVM base, RegLLVM reg, RegLLVM offReg, unsigned cond);
llvm::MCInst t2ldrbrs(RegLLVM base, RegLLVM reg, RegLLVM offReg, unsigned lsl);
llvm::MCInst t2ldrbrs(RegLLVM base, RegLLVM reg, RegLLVM offReg, unsigned lsl,
                      unsigned cond);

// load exclusif

llvm::MCInst ldrexb(RegLLVM base, RegLLVM reg);
llvm::MCInst ldrexb(RegLLVM base, RegLLVM reg, unsigned cond);
llvm::MCInst t2ldrexb(RegLLVM base, RegLLVM reg);
llvm::MCInst t2ldrexb(RegLLVM base, RegLLVM reg, unsigned cond);

llvm::MCInst ldrexh(RegLLVM base, RegLLVM reg);
llvm::MCInst ldrexh(RegLLVM base, RegLLVM reg, unsigned cond);
llvm::MCInst t2ldrexh(RegLLVM base, RegLLVM reg);
llvm::MCInst t2ldrexh(RegLLVM base, RegLLVM reg, unsigned cond);

llvm::MCInst ldrex(RegLLVM base, RegLLVM reg);
llvm::MCInst ldrex(RegLLVM base, RegLLVM reg, unsigned cond);
llvm::MCInst t2ldrex(RegLLVM base, RegLLVM reg);
llvm::MCInst t2ldrex(RegLLVM base, RegLLVM reg, unsigned cond);

llvm::MCInst ldrexd(RegLLVM base, RegLLVM base2, RegLLVM reg);
llvm::MCInst ldrexd(RegLLVM base, RegLLVM base2, RegLLVM reg, unsigned cond);
llvm::MCInst t2ldrexd(RegLLVM base, RegLLVM base2, RegLLVM reg);
llvm::MCInst t2ldrexd(RegLLVM base, RegLLVM base2, RegLLVM reg, unsigned cond);

// load multiple

llvm::MCInst ldmia(RegLLVM base, unsigned int regMask, bool wback);
llvm::MCInst ldmia(RegLLVM base, unsigned int regMask, bool wback,
                   unsigned cond);
llvm::MCInst t2ldmia(RegLLVM base, unsigned int regMask, bool wback);
llvm::MCInst t2ldmia(RegLLVM base, unsigned int regMask, bool wback,
                     unsigned cond);
llvm::MCInst t2ldmdb(RegLLVM base, unsigned int regMask, bool wback);
llvm::MCInst t2ldmdb(RegLLVM base, unsigned int regMask, bool wback,
                     unsigned cond);

llvm::MCInst vldmia(RegLLVM base, RegLLVM reg, unsigned int nreg, bool wback);
llvm::MCInst vldmia(RegLLVM base, RegLLVM reg, unsigned int nreg, bool wback,
                    unsigned cond);

// StoreInst
llvm::MCInst pushr1(RegLLVM reg, unsigned cond);

llvm::MCInst stri12(RegLLVM reg, RegLLVM base, sword offset);
llvm::MCInst stri12(RegLLVM reg, RegLLVM base, sword offset, unsigned cond);
llvm::MCInst t2stri8(RegLLVM reg, RegLLVM base, sword offset);
llvm::MCInst t2stri8(RegLLVM reg, RegLLVM base, sword offset, unsigned cond);
llvm::MCInst t2stri12(RegLLVM reg, RegLLVM base, sword offset);
llvm::MCInst t2stri12(RegLLVM reg, RegLLVM base, sword offset, unsigned cond);
llvm::MCInst t2strPost(RegLLVM reg, RegLLVM base, sword offset);
llvm::MCInst t2strPost(RegLLVM reg, RegLLVM base, sword offset, unsigned cond);
llvm::MCInst t2strPre(RegLLVM reg, RegLLVM base, sword offset);
llvm::MCInst t2strPre(RegLLVM reg, RegLLVM base, sword offset, unsigned cond);

// store multiple

llvm::MCInst stmia(RegLLVM base, unsigned int regMask, bool wback);
llvm::MCInst stmia(RegLLVM base, unsigned int regMask, bool wback,
                   unsigned cond);
llvm::MCInst t2stmia(RegLLVM base, unsigned int regMask, bool wback);
llvm::MCInst t2stmia(RegLLVM base, unsigned int regMask, bool wback,
                     unsigned cond);
llvm::MCInst t2stmdb(RegLLVM base, unsigned int regMask, bool wback);
llvm::MCInst t2stmdb(RegLLVM base, unsigned int regMask, bool wback,
                     unsigned cond);

llvm::MCInst vstmia(RegLLVM base, RegLLVM reg, unsigned int nreg, bool wback);
llvm::MCInst vstmia(RegLLVM base, RegLLVM reg, unsigned int nreg, bool wback,
                    unsigned cond);

// mov register - register

llvm::MCInst movr(RegLLVM dst, RegLLVM src);
llvm::MCInst movr(RegLLVM dst, RegLLVM src, unsigned cond);
llvm::MCInst tmovr(RegLLVM dst, RegLLVM src);
llvm::MCInst tmovr(RegLLVM dst, RegLLVM src, unsigned cond);

// mov Immediate

llvm::MCInst movi(RegLLVM dst, rword imm);
llvm::MCInst movi(RegLLVM dst, rword imm, unsigned cond);

llvm::MCInst t2movi(RegLLVM dst, rword imm);
llvm::MCInst t2movi(RegLLVM dst, rword imm, unsigned cond);

// branch

llvm::MCInst branch(sword offset);
llvm::MCInst bcc(sword offset, unsigned cond);
llvm::MCInst tbranch(sword offset);
llvm::MCInst tbranchIT(sword offset, unsigned cond);
llvm::MCInst tbcc(sword offset, unsigned cond);
llvm::MCInst t2branch(sword offset);
llvm::MCInst t2branchIT(sword offset, unsigned cond);
llvm::MCInst t2bcc(sword offset, unsigned cond);

llvm::MCInst tbx(RegLLVM reg);
llvm::MCInst tbx(RegLLVM reg, unsigned cond);

// flags mov
llvm::MCInst mrs(RegLLVM dst);
llvm::MCInst mrs(RegLLVM dst, unsigned cond);
llvm::MCInst msr(RegLLVM dst);
llvm::MCInst msr(RegLLVM dst, unsigned cond);
llvm::MCInst t2mrs(RegLLVM dst);
llvm::MCInst t2mrs(RegLLVM dst, unsigned cond);
llvm::MCInst t2msr(RegLLVM dst);
llvm::MCInst t2msr(RegLLVM dst, unsigned cond);

llvm::MCInst vmrs(RegLLVM dst);
llvm::MCInst vmrs(RegLLVM dst, unsigned cond);
llvm::MCInst vmsr(RegLLVM dst);
llvm::MCInst vmsr(RegLLVM dst, unsigned cond);

// get relative address

llvm::MCInst adr(RegLLVM reg, rword offset);
llvm::MCInst adr(RegLLVM reg, rword offset, unsigned cond);
llvm::MCInst t2adr(RegLLVM reg, sword offset);
llvm::MCInst t2adr(RegLLVM reg, sword offset, unsigned cond);

// add

llvm::MCInst add(RegLLVM dst, RegLLVM src, rword imm);
llvm::MCInst add(RegLLVM dst, RegLLVM src, rword imm, unsigned cond);
llvm::MCInst t2add(RegLLVM dst, RegLLVM src, rword imm);
llvm::MCInst t2add(RegLLVM dst, RegLLVM src, rword imm, unsigned cond);

llvm::MCInst addrsi(RegLLVM dst, RegLLVM src, RegLLVM srcOff, unsigned shift,
                    unsigned shiftType);
llvm::MCInst addrsi(RegLLVM dst, RegLLVM src, RegLLVM srcOff, unsigned shift,
                    unsigned shiftType, unsigned cond);
llvm::MCInst t2addrsi(RegLLVM dst, RegLLVM src, RegLLVM srcOff, unsigned shift,
                      unsigned shiftType);
llvm::MCInst t2addrsi(RegLLVM dst, RegLLVM src, RegLLVM srcOff, unsigned shift,
                      unsigned shiftType, unsigned cond);

llvm::MCInst addr(RegLLVM dst, RegLLVM src, RegLLVM src2);
llvm::MCInst addr(RegLLVM dst, RegLLVM src, RegLLVM src2, unsigned cond);
llvm::MCInst t2addr(RegLLVM dst, RegLLVM src, RegLLVM src2);
llvm::MCInst t2addr(RegLLVM dst, RegLLVM src, RegLLVM src2, unsigned cond);

// sub

llvm::MCInst sub(RegLLVM dst, RegLLVM src, rword imm);
llvm::MCInst sub(RegLLVM dst, RegLLVM src, rword imm, unsigned cond);
llvm::MCInst t2sub(RegLLVM dst, RegLLVM src, rword imm);
llvm::MCInst t2sub(RegLLVM dst, RegLLVM src, rword imm, unsigned cond);

llvm::MCInst subrsi(RegLLVM dst, RegLLVM src, RegLLVM srcOff, unsigned shift,
                    unsigned shiftType);
llvm::MCInst subrsi(RegLLVM dst, RegLLVM src, RegLLVM srcOff, unsigned shift,
                    unsigned shiftType, unsigned cond);
llvm::MCInst t2subrsi(RegLLVM dst, RegLLVM src, RegLLVM srcOff, unsigned shift,
                      unsigned shiftType);
llvm::MCInst t2subrsi(RegLLVM dst, RegLLVM src, RegLLVM srcOff, unsigned shift,
                      unsigned shiftType, unsigned cond);

llvm::MCInst subr(RegLLVM dst, RegLLVM src, RegLLVM src2);
llvm::MCInst subr(RegLLVM dst, RegLLVM src, RegLLVM src2, unsigned cond);
llvm::MCInst t2subr(RegLLVM dst, RegLLVM src, RegLLVM src2);
llvm::MCInst t2subr(RegLLVM dst, RegLLVM src, RegLLVM src2, unsigned cond);

// bit clear

llvm::MCInst bic(RegLLVM dst, RegLLVM src, rword imm);
llvm::MCInst bic(RegLLVM dst, RegLLVM src, rword imm, unsigned cond);
llvm::MCInst t2bic(RegLLVM dst, RegLLVM src, rword imm);
llvm::MCInst t2bic(RegLLVM dst, RegLLVM src, rword imm, unsigned cond);

// or

llvm::MCInst orri(RegLLVM dest, RegLLVM reg, rword imm);
llvm::MCInst orri(RegLLVM dest, RegLLVM reg, rword imm, unsigned cond);
llvm::MCInst t2orri(RegLLVM dest, RegLLVM reg, rword imm);
llvm::MCInst t2orri(RegLLVM dest, RegLLVM reg, rword imm, unsigned cond);

llvm::MCInst orrshift(RegLLVM dest, RegLLVM reg, RegLLVM reg2,
                      unsigned int lshift);
llvm::MCInst orrshift(RegLLVM dest, RegLLVM reg, RegLLVM reg2,
                      unsigned int lshift, unsigned cond);
llvm::MCInst t2orrshift(RegLLVM dest, RegLLVM reg, RegLLVM reg2,
                        unsigned int lshift);
llvm::MCInst t2orrshift(RegLLVM dest, RegLLVM reg, RegLLVM reg2,
                        unsigned int lshift, unsigned cond);

// compare

llvm::MCInst cmp(RegLLVM src, rword imm);
llvm::MCInst cmp(RegLLVM src, rword imm, unsigned cond);
llvm::MCInst t2cmp(RegLLVM src, rword imm);
llvm::MCInst t2cmp(RegLLVM src, rword imm, unsigned cond);

// misc

// see Utils/ARMBaseInfo.h llvm::ARMCC::CondCodes and llvm::ARM::PredBlockMask
llvm::MCInst t2it(unsigned int cond, unsigned pred);
llvm::MCInst nop();
llvm::MCInst bkpt(unsigned int value);

// High level layer 2

std::unique_ptr<RelocatableInst> Popr1(CPUMode cpuMode, Reg reg);
std::unique_ptr<RelocatableInst> Pushr1(CPUMode cpuMode, Reg reg);

std::unique_ptr<RelocatableInst> Add(CPUMode cpuMode, RegLLVM dst, RegLLVM src,
                                     Constant cst);
std::unique_ptr<RelocatableInst> Addr(CPUMode cpuMode, RegLLVM dst, RegLLVM src,
                                      RegLLVM src2);
std::unique_ptr<RelocatableInst> Subr(CPUMode cpuMode, RegLLVM dst, RegLLVM src,
                                      RegLLVM src2);
std::unique_ptr<RelocatableInst> Addrs(CPUMode cpuMode, RegLLVM dst,
                                       RegLLVM src, RegLLVM srcOff,
                                       unsigned shift, unsigned shiftType);
std::unique_ptr<RelocatableInst> Subrs(CPUMode cpuMode, RegLLVM dst,
                                       RegLLVM src, RegLLVM srcOff,
                                       unsigned shift, unsigned shiftType);

std::unique_ptr<RelocatableInst>
LdmIA(CPUMode cpuMode, RegLLVM base, unsigned int regMask, bool wback = false);
std::unique_ptr<RelocatableInst>
StmIA(CPUMode cpuMode, RegLLVM base, unsigned int regMask, bool wback = false);
std::unique_ptr<RelocatableInst> VLdmIA(CPUMode cpuMode, RegLLVM base,
                                        unsigned int reg, unsigned int nreg,
                                        bool wback = false);
std::unique_ptr<RelocatableInst> VStmIA(CPUMode cpuMode, RegLLVM base,
                                        unsigned int reg, unsigned int nreg,
                                        bool wback = false);

std::unique_ptr<RelocatableInst> Mrs(CPUMode cpuMode, Reg reg);
std::unique_ptr<RelocatableInst> Msr(CPUMode cpuMode, Reg reg);
std::unique_ptr<RelocatableInst> Vmrs(CPUMode cpuMode, Reg reg);
std::unique_ptr<RelocatableInst> Vmsr(CPUMode cpuMode, Reg reg);
std::unique_ptr<RelocatableInst> Bkpt(CPUMode cpuMode, unsigned int value);

std::unique_ptr<RelocatableInst> T2it(CPUMode cpuMode, unsigned int cond,
                                      unsigned pred);

std::vector<std::unique_ptr<RelocatableInst>>
Addc(CPUMode cpuMode, RegLLVM dst, RegLLVM src, Constant val, RegLLVM temp);
std::vector<std::unique_ptr<RelocatableInst>> Addc(CPUMode cpuMode, RegLLVM dst,
                                                   RegLLVM src, Constant val,
                                                   RegLLVM temp,
                                                   unsigned int cond);

std::unique_ptr<RelocatableInst> Cmp(CPUMode cpuMode, RegLLVM src, rword imm);

std::unique_ptr<RelocatableInst> Branch(CPUMode cpuMode, sword offset,
                                        bool addBranchLen = false);
std::unique_ptr<RelocatableInst> BranchCC(CPUMode cpuMode, sword offset,
                                          unsigned cond,
                                          bool withinITBlock = false,
                                          bool addBranchLen = false);

} // namespace QBDI

#endif
