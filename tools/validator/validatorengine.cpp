/*
 * This file is part of QBDI.
 *
 * Copyright 2017 Quarkslab
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
#include "validatorengine.h"
#include <algorithm>
#include <cstring>
#include <inttypes.h>


LogEntry::LogEntry(uint64_t execID, QBDI::rword address, const char* disassembly) {
    this->execID = execID;
    this->address = address;
    int len = strlen(disassembly) + 1;
    this->disassembly = new char[len];
    strncpy(this->disassembly, disassembly, len);
    this->transfer = 0;
}

LogEntry::LogEntry(const LogEntry& copy) {
    this->execID = copy.execID;
    this->address = copy.address;
    int len = strlen(copy.disassembly) + 1;
    this->disassembly = new char[len];
    strncpy(this->disassembly, copy.disassembly, len);
    this->transfer = copy.transfer;
    this->errorIDs.reserve(copy.errorIDs.size());
    for(ssize_t eID : copy.errorIDs) {
        this->errorIDs.push_back(eID);
    }
}

LogEntry::~LogEntry() {
    delete[] this->disassembly;
}

std::pair<QBDI::rword, QBDI::rword> getValidOffsetRange(QBDI::rword address, pid_t pid) {
    std::vector<QBDI::MemoryMap> maps = QBDI::getRemoteProcessMaps(pid);
    for(QBDI::MemoryMap &m : maps) {
        if(m.start <= address && m.end > address) {
            return std::make_pair(address - m.start, m.end - address - 1);
        }
    }
    return std::make_pair(0, 0);
}

ssize_t ValidatorEngine::logEntryLookup(uint64_t execID) {
    size_t upper = savedLogs.size();
    size_t lower = 0;
    while(upper != lower) {
        size_t i = (upper + lower)/2;
        if(this->savedLogs[i].execID < execID) {
            lower = i;
        }
        else if(this->savedLogs[i].execID > execID) {
            upper = i;
        }
        else {
            return (ssize_t) i;
        }
    }
    return -1;
}

void ValidatorEngine::outputLogEntry(const LogEntry& entry) {
    fprintf(stderr, "ExecID: %" PRIu64 " \t%016" PRIRWORD ": %s\n", entry.execID, entry.address, entry.disassembly);
    if(entry.transfer != 0)
        fprintf(stderr, "\tCaused a transfer to address 0x%" PRIRWORD "\n", entry.transfer);
    for(ssize_t eID : entry.errorIDs) {
        if(errors[eID].severity == ErrorSeverity::NoImpact)
            fprintf(stderr, "\tError with no impact ");
        else if(errors[eID].severity == ErrorSeverity::NonCritical)
            fprintf(stderr, "\tError with non critical impact ");
        else if(errors[eID].severity == ErrorSeverity::Critical)
            fprintf(stderr, "\tError with critical impact ");
        fprintf(stderr, "on %s: 0x%" PRIRWORD " (real) != 0x%" PRIRWORD " (qbdi)\n", errors[eID].regName, errors[eID].real, errors[eID].qbdi);
    }
}

template<typename T> ssize_t ValidatorEngine::diff(const char* regName, T real, T qbdi) {
    // No diff, no error
    if(real == qbdi) {
        return -1;
    }

    DiffError e;
    e.regName = regName;
    e.real = (QBDI::rword) real;
    e.qbdi = (QBDI::rword) qbdi;
    e.severity = ErrorSeverity::NoImpact;
    e.causeExecID = execID;
    e.cascadeID = execID;

    // If lastLogEntry has at least one error, consider this new error to be a cascade
    if(lastLogEntry != nullptr && lastLogEntry->errorIDs.size() > 0) {
        e.cascadeID = errors[lastLogEntry->errorIDs[0]].cascadeID;
        // Upgrade previous error severity to non critical because they caused a cascade
        for(ssize_t eID : lastLogEntry->errorIDs) {
            errors[eID].severity = ErrorSeverity::NonCritical;
        }
    }
    else {
        // If equivalent error exist, consider this new error to be a cascade
        for(DiffError &prevError : errors) {
            if(e.real == prevError.real && e.qbdi == prevError.qbdi) {
                e.cascadeID = prevError.cascadeID;
                // Upgrade previous error severity to non critical because they caused a cascade
                prevError.severity = ErrorSeverity::NonCritical;
                break;
            }
        }
    }
    // else new cascadeID
    errors.push_back(e);
    return errors.size() - 1;
}

ssize_t ValidatorEngine::diffGPR(unsigned regID, QBDI::rword real, QBDI::rword qbdi) {
    // No diff, no error
    if(real == qbdi) {
        return -1;
    }

    // First try to explain the diff using the diffmaps
    for(const DiffMap& d : diffMaps) {
        if(real - d.real == qbdi - d.qbdi &&
           ((d.real >= real && d.real - real <= d.lowerOffset) ||
           (real >= d.real && real - d.real <= d.upperOffset))) {
            // Explained, no error
            return -1;
        }
    }

    // If this error is just propagating from the previous state, just copy the pointer
    if(lastLogEntry != nullptr) {
        for(ssize_t eID : lastLogEntry->errorIDs) {
            if(strcmp(QBDI::GPR_NAMES[regID], errors[eID].regName) == 0 &&
               real == errors[eID].real && qbdi == errors[eID].qbdi) {
                return eID;
            }
        }
    }

    // New error at the begining of execution or after an ExecTransfer, add to diffmaps
    if(lastLogEntry == nullptr || curLogEntry->transfer != 0) {
        DiffMap d;
        d.real = real;
        d.qbdi = qbdi;
        std::pair<QBDI::rword, QBDI::rword> rangeDbg = getValidOffsetRange(real, debugged);
        std::pair<QBDI::rword, QBDI::rword> rangeInstr = getValidOffsetRange(qbdi, instrumented);
        d.lowerOffset = std::min(rangeDbg.first, rangeInstr.first);
        d.upperOffset = std::min(rangeDbg.second, rangeInstr.second);
        d.causeExecID = execID;
        diffMaps.push_back(d);
        return -1;
    }

    // Else normal error diff
    return diff<QBDI::rword>(QBDI::GPR_NAMES[regID], real, qbdi);
}

template<typename T> ssize_t ValidatorEngine::diffSPR(const char* regName, T real, T qbdi) {
    // No diff, no error
    if(real == qbdi) {
        return -1;
    }

    // First try to explain the diff using the exact diff of the diffmaps
    for(const DiffMap& d : diffMaps) {
        if(d.lowerOffset == 0 && d.upperOffset == 0 && real == d.real && qbdi == d.qbdi) {
            return -1;
        }
    }

    // If this error is just propagating from the previous state, just copy the pointer
    if(lastLogEntry != nullptr) {
        for(ssize_t eID : lastLogEntry->errorIDs) {
            if(strcmp(regName, errors[eID].regName) == 0 &&
               real == errors[eID].real && qbdi == errors[eID].qbdi) {
                return eID;
            }
        }
    }

    // New error at the begining of execution or after an ExecTransfer, add to diffmaps as an exact diff
    if(lastLogEntry == nullptr || curLogEntry->transfer != 0) {
        DiffMap d;
        d.real = real;
        d.qbdi = qbdi;
        d.lowerOffset = 0;
        d.upperOffset = 0;
        d.causeExecID = execID;
        diffMaps.push_back(d);
        return -1;
    }

    // Else normal error diff
    return diff<QBDI::rword>(regName, real, qbdi);
}

void ValidatorEngine::signalNewState(QBDI::rword address, const char* mnemonic, const char* disassembly, 
                        const QBDI::GPRState *gprStateDbg, const QBDI::FPRState *fprStateDbg, 
                        const QBDI::GPRState *gprStateInstr, const QBDI::FPRState *fprStateInstr) {
    ssize_t e;

    if(curLogEntry != nullptr) {
        #if defined(QBDI_ARCH_ARM)
        if((e = diffGPR(0,  gprStateDbg->r0,  gprStateInstr->r0))   != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(1,  gprStateDbg->r1,  gprStateInstr->r1))   != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(2,  gprStateDbg->r2,  gprStateInstr->r2))   != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(3,  gprStateDbg->r3,  gprStateInstr->r3))   != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(4,  gprStateDbg->r4,  gprStateInstr->r4))   != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(5,  gprStateDbg->r5,  gprStateInstr->r5))   != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(6,  gprStateDbg->r6,  gprStateInstr->r6))   != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(7,  gprStateDbg->r7,  gprStateInstr->r7))   != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(8,  gprStateDbg->r8,  gprStateInstr->r8))   != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(9,  gprStateDbg->r9,  gprStateInstr->r9))   != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(9,  gprStateDbg->r9,  gprStateInstr->r9))   != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(10, gprStateDbg->r10, gprStateInstr->r10))  != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(11, gprStateDbg->r12, gprStateInstr->r12))  != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(12, gprStateDbg->fp, gprStateInstr->fp))   != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(13, gprStateDbg->sp, gprStateInstr->sp))   != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(14, gprStateDbg->lr, gprStateInstr->lr))   != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(16, gprStateDbg->cpsr, gprStateInstr->cpsr)) != -1) curLogEntry->errorIDs.push_back(e);
        // FPR
        union {
            float s;
            uint32_t i;
        } sreg1, sreg2;
        #define DIFF_S(name, dbg, instr) \
            sreg1.s = dbg; \
            sreg2.s = instr; \
            if((e = diffSPR(name,   sreg1.i, sreg2.i))  != -1) curLogEntry->errorIDs.push_back(e);
        DIFF_S("s0", fprStateDbg->s[0], fprStateInstr->s[0]);
        DIFF_S("s1", fprStateDbg->s[1], fprStateInstr->s[1]);
        DIFF_S("s2", fprStateDbg->s[2], fprStateInstr->s[2]);
        DIFF_S("s3", fprStateDbg->s[3], fprStateInstr->s[3]);
        DIFF_S("s4", fprStateDbg->s[4], fprStateInstr->s[4]);
        DIFF_S("s5", fprStateDbg->s[5], fprStateInstr->s[5]);
        DIFF_S("s6", fprStateDbg->s[6], fprStateInstr->s[6]);
        DIFF_S("s7", fprStateDbg->s[7], fprStateInstr->s[7]);
        DIFF_S("s8", fprStateDbg->s[8], fprStateInstr->s[8]);
        DIFF_S("s9", fprStateDbg->s[9], fprStateInstr->s[9]);
        DIFF_S("s10", fprStateDbg->s[10], fprStateInstr->s[10]);
        DIFF_S("s11", fprStateDbg->s[11], fprStateInstr->s[11]);
        DIFF_S("s12", fprStateDbg->s[12], fprStateInstr->s[12]);
        DIFF_S("s13", fprStateDbg->s[13], fprStateInstr->s[13]);
        DIFF_S("s14", fprStateDbg->s[14], fprStateInstr->s[14]);
        DIFF_S("s15", fprStateDbg->s[15], fprStateInstr->s[15]);
        DIFF_S("s16", fprStateDbg->s[16], fprStateInstr->s[16]);
        DIFF_S("s17", fprStateDbg->s[17], fprStateInstr->s[17]);
        DIFF_S("s18", fprStateDbg->s[18], fprStateInstr->s[18]);
        DIFF_S("s19", fprStateDbg->s[19], fprStateInstr->s[19]);
        DIFF_S("s20", fprStateDbg->s[20], fprStateInstr->s[20]);
        DIFF_S("s21", fprStateDbg->s[21], fprStateInstr->s[21]);
        DIFF_S("s22", fprStateDbg->s[22], fprStateInstr->s[22]);
        DIFF_S("s23", fprStateDbg->s[23], fprStateInstr->s[23]);
        DIFF_S("s24", fprStateDbg->s[24], fprStateInstr->s[24]);
        DIFF_S("s25", fprStateDbg->s[25], fprStateInstr->s[25]);
        DIFF_S("s26", fprStateDbg->s[26], fprStateInstr->s[26]);
        DIFF_S("s27", fprStateDbg->s[27], fprStateInstr->s[27]);
        DIFF_S("s28", fprStateDbg->s[28], fprStateInstr->s[28]);
        DIFF_S("s29", fprStateDbg->s[29], fprStateInstr->s[29]);
        DIFF_S("s30", fprStateDbg->s[30], fprStateInstr->s[30]);
        DIFF_S("s31", fprStateDbg->s[31], fprStateInstr->s[31]);
        #undef DIFF_S
        #elif defined(QBDI_ARCH_X86_64)
        // GPR
        if((e = diffGPR(0,  gprStateDbg->rax, gprStateInstr->rax))       != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(1,  gprStateDbg->rbx, gprStateInstr->rbx))       != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(2,  gprStateDbg->rcx, gprStateInstr->rcx))       != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(3,  gprStateDbg->rdx, gprStateInstr->rdx))       != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(4,  gprStateDbg->rsi, gprStateInstr->rsi))       != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(5,  gprStateDbg->rdi, gprStateInstr->rdi))       != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(6,  gprStateDbg->r8,  gprStateInstr->r8))        != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(7,  gprStateDbg->r9,  gprStateInstr->r9))        != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(8,  gprStateDbg->r10, gprStateInstr->r10))       != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(9,  gprStateDbg->r11, gprStateInstr->r11))       != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(10, gprStateDbg->r12, gprStateInstr->r12))       != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(11, gprStateDbg->r13, gprStateInstr->r13))       != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(12, gprStateDbg->r14, gprStateInstr->r14))       != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(13, gprStateDbg->r15, gprStateInstr->r15))       != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(14, gprStateDbg->rbp, gprStateInstr->rbp))       != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diffGPR(15, gprStateDbg->rsp, gprStateInstr->rsp))       != -1) curLogEntry->errorIDs.push_back(e);
        // FPR
        union {
            QBDI::MMSTReg st;
            struct {
                uint32_t m0;
                uint32_t m1;
                uint16_t e;
                uint16_t reserved;
            };
        } streg1, streg2;
        #define DIFF_ST(name, dbg, instr) \
            streg1.st = dbg; \
            streg2.st = instr; \
            if((e = diffSPR(name ".m[0:32]",   streg1.m0, streg2.m0))  != -1) curLogEntry->errorIDs.push_back(e); \
            if((e = diffSPR(name ".m[32:64]",  streg1.m1, streg2.m1))  != -1) curLogEntry->errorIDs.push_back(e); \
            if((e = diffSPR(name ".e",         streg1.e,  streg2.e))   != -1) curLogEntry->errorIDs.push_back(e);
        DIFF_ST("st0", fprStateDbg->stmm0, fprStateInstr->stmm0);
        DIFF_ST("st1", fprStateDbg->stmm1, fprStateInstr->stmm1);
        DIFF_ST("st2", fprStateDbg->stmm2, fprStateInstr->stmm2);
        DIFF_ST("st3", fprStateDbg->stmm3, fprStateInstr->stmm3);
        DIFF_ST("st4", fprStateDbg->stmm4, fprStateInstr->stmm4);
        DIFF_ST("st5", fprStateDbg->stmm5, fprStateInstr->stmm5);
        DIFF_ST("st6", fprStateDbg->stmm6, fprStateInstr->stmm6);
        DIFF_ST("st7", fprStateDbg->stmm7, fprStateInstr->stmm7);
        union {
            char xmm[16];
            struct {
                uint32_t m0;
                uint32_t m1;
                uint32_t m2;
                uint32_t m3;
            };
        } xmmreg1, xmmreg2;
        #define DIFF_XMM(name, dbg, instr) \
            memcpy(xmmreg1.xmm, dbg, 16); \
            memcpy(xmmreg2.xmm, instr, 16); \
            if((e = diffSPR(name "[0:32]",   xmmreg1.m0, xmmreg2.m0))  != -1) curLogEntry->errorIDs.push_back(e); \
            if((e = diffSPR(name "[32:64]",  xmmreg1.m1, xmmreg2.m1))  != -1) curLogEntry->errorIDs.push_back(e); \
            if((e = diffSPR(name "[64:96]",  xmmreg1.m2, xmmreg2.m2))  != -1) curLogEntry->errorIDs.push_back(e); \
            if((e = diffSPR(name "[96:128]", xmmreg1.m3, xmmreg2.m3))  != -1) curLogEntry->errorIDs.push_back(e); 
        DIFF_XMM("xmm0", fprStateDbg->xmm0, fprStateInstr->xmm0);
        DIFF_XMM("xmm1", fprStateDbg->xmm1, fprStateInstr->xmm1);
        DIFF_XMM("xmm2", fprStateDbg->xmm2, fprStateInstr->xmm2);
        DIFF_XMM("xmm3", fprStateDbg->xmm3, fprStateInstr->xmm3);
        DIFF_XMM("xmm4", fprStateDbg->xmm4, fprStateInstr->xmm4);
        DIFF_XMM("xmm5", fprStateDbg->xmm5, fprStateInstr->xmm5);
        DIFF_XMM("xmm6", fprStateDbg->xmm6, fprStateInstr->xmm6);
        DIFF_XMM("xmm7", fprStateDbg->xmm7, fprStateInstr->xmm7);
        DIFF_XMM("xmm8", fprStateDbg->xmm8, fprStateInstr->xmm8);
        DIFF_XMM("xmm9", fprStateDbg->xmm9, fprStateInstr->xmm9);
        DIFF_XMM("xmm10", fprStateDbg->xmm10, fprStateInstr->xmm10);
        DIFF_XMM("xmm11", fprStateDbg->xmm11, fprStateInstr->xmm11);
        DIFF_XMM("xmm12", fprStateDbg->xmm12, fprStateInstr->xmm12);
        DIFF_XMM("xmm13", fprStateDbg->xmm13, fprStateInstr->xmm13);
        DIFF_XMM("xmm14", fprStateDbg->xmm14, fprStateInstr->xmm14);
        DIFF_XMM("xmm15", fprStateDbg->xmm15, fprStateInstr->xmm15);
        // FIXME
#if 0
        DIFF_XMM("ymm0", fprStateDbg->ymm0, fprStateInstr->ymm0);
        DIFF_XMM("ymm1", fprStateDbg->ymm1, fprStateInstr->ymm1);
        DIFF_XMM("ymm2", fprStateDbg->ymm2, fprStateInstr->ymm2);
        DIFF_XMM("ymm3", fprStateDbg->ymm3, fprStateInstr->ymm3);
        DIFF_XMM("ymm4", fprStateDbg->ymm4, fprStateInstr->ymm4);
        DIFF_XMM("ymm5", fprStateDbg->ymm5, fprStateInstr->ymm5);
        DIFF_XMM("ymm6", fprStateDbg->ymm6, fprStateInstr->ymm6);
        DIFF_XMM("ymm7", fprStateDbg->ymm7, fprStateInstr->ymm7);
        DIFF_XMM("ymm8", fprStateDbg->ymm8, fprStateInstr->ymm8);
        DIFF_XMM("ymm9", fprStateDbg->ymm9, fprStateInstr->ymm9);
        DIFF_XMM("ymm10", fprStateDbg->ymm10, fprStateInstr->ymm10);
        DIFF_XMM("ymm11", fprStateDbg->ymm11, fprStateInstr->ymm11);
        DIFF_XMM("ymm12", fprStateDbg->ymm12, fprStateInstr->ymm12);
        DIFF_XMM("ymm13", fprStateDbg->ymm13, fprStateInstr->ymm13);
        DIFF_XMM("ymm14", fprStateDbg->ymm14, fprStateInstr->ymm14);
        DIFF_XMM("ymm15", fprStateDbg->ymm15, fprStateInstr->ymm15);
#endif
        if((e = diff("fcw", fprStateDbg->rfcw, fprStateInstr->rfcw))                 != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diff("fsw", fprStateDbg->rfsw, fprStateInstr->rfsw))                 != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diff("ftw", fprStateDbg->ftw, fprStateInstr->ftw))                   != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diff("fop", fprStateDbg->fop, fprStateInstr->fop))                   != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diff("mxcsr", fprStateDbg->mxcsr, fprStateInstr->mxcsr))             != -1) curLogEntry->errorIDs.push_back(e);
        if((e = diff("mxcsrmask", fprStateDbg->mxcsrmask, fprStateInstr->mxcsrmask)) != -1) curLogEntry->errorIDs.push_back(e);
        #undef DIFF_ST
        #undef DIFF_XMM
        // Clear the auxilliary carry flag which generates noisy output
        QBDI::rword eflagsDbg = (gprStateDbg->eflags) & (gprStateDbg->eflags ^ 0x4);
        QBDI::rword eflagsInstr = (gprStateInstr->eflags) & (gprStateInstr->eflags ^ 0x4);
        if((e = diffGPR(17, eflagsDbg, eflagsInstr)) != -1) curLogEntry->errorIDs.push_back(e);
        #endif

        // If this logEntry generated at least one new error, save it
        for(const ssize_t eID : curLogEntry->errorIDs) {
            if(errors[eID].causeExecID == execID) {
                savedLogs.push_back(*curLogEntry);
                break;
            }
        }
    }
    
    if(lastLogEntry != nullptr) {
        if(verbosity == LogVerbosity::Full) {
            outputLogEntry(*lastLogEntry);
        }
        delete lastLogEntry;
    }
    lastLogEntry = curLogEntry;

    execID++;

    coverage[std::string(mnemonic)]++;
    curLogEntry = new LogEntry(execID, address, disassembly);
}

void ValidatorEngine::signalExecTransfer(QBDI::rword address) {
    if(curLogEntry != nullptr) {
        curLogEntry->transfer = address;
    }
}

void ValidatorEngine::signalCriticalState() {
    // Mark current errors as critical
    if(lastLogEntry != nullptr) {
        for(const ssize_t eID : lastLogEntry->errorIDs) {
            errors[eID].severity = ErrorSeverity::Critical;
        }
    }
}

void ValidatorEngine::flushLastLog() {
    if(lastLogEntry != nullptr) {
        if(verbosity == LogVerbosity::Full) {
            outputLogEntry(*lastLogEntry);
        }
        delete lastLogEntry;
    }
    if(curLogEntry != nullptr) {
        if(verbosity == LogVerbosity::Full) {
            outputLogEntry(*curLogEntry);
        }
        delete curLogEntry;
    }
}

void ValidatorEngine::logCascades() {
    std::vector<Cascade> cascades;
    if(verbosity >= LogVerbosity::Stat) {
        size_t noImpactCount = 0;
        size_t nonCriticalCount = 0;
        size_t criticalCount = 0;
        fprintf(stderr, "Stats\n");
        fprintf(stderr, "=====\n\n");
        fprintf(stderr, "Executed %" PRIu64 " total instructions\n", execID);
        fprintf(stderr, "Executed %zu unique instructions\n", coverage.size());
        fprintf(stderr, "Encountered %zu difference mappings\n", diffMaps.size());
        fprintf(stderr, "Encountered %zu errors:\n", errors.size());
        // Compute error stats, assemble cascades
        for(const DiffError& error : errors) {
            bool found = false;
            for(size_t i = 0; i < cascades.size(); i++) {
                if(cascades[i].cascadeID == error.cascadeID) {
                    found = true;
                    cascades[i].execIDs.push_back(error.causeExecID);
                    if(error.severity > cascades[i].severity) {
                        cascades[i].severity = error.severity;
                    }
                }
            }
            if(!found) {
                cascades.push_back(Cascade {
                    error.cascadeID,
                    this->savedLogs[logEntryLookup(error.causeExecID)].address,
                    error.severity,
                    std::vector<uint64_t>(),
                    std::vector<uint64_t>()
                });
                cascades.back().execIDs.push_back(error.causeExecID);
            }

            if(error.severity == ErrorSeverity::NoImpact) noImpactCount++;
            else if(error.severity == ErrorSeverity::NonCritical) nonCriticalCount++;
            else if(error.severity == ErrorSeverity::Critical) criticalCount++;
        }
        fprintf(stderr, "\tNo impact errors: %zu\n", noImpactCount);
        fprintf(stderr, "\tNon critical errors: %zu\n", nonCriticalCount);
        fprintf(stderr, "\tCritical errors: %zu\n", criticalCount);
        fprintf(stderr, "Encountered %zu error cascades:\n", cascades.size());
        // Compute cascade stats
        noImpactCount = 0;
        nonCriticalCount = 0;
        criticalCount = 0;
        for(size_t i = 0; i < cascades.size(); i++) {
            if(cascades[i].severity == ErrorSeverity::NoImpact) noImpactCount++;
            else if(cascades[i].severity == ErrorSeverity::NonCritical) nonCriticalCount++;
            else if(cascades[i].severity == ErrorSeverity::Critical) criticalCount++;
        }
        fprintf(stderr, "\tNo impact cascades: %zu\n", noImpactCount);
        fprintf(stderr, "\tNon critical cascades: %zu\n", nonCriticalCount);
        fprintf(stderr, "\tCritical cascades: %zu\n", criticalCount);
    }
    if(verbosity >= LogVerbosity::Summary) {
        fprintf(stderr, "\n\n");
        fprintf(stderr, "Error cascades:\n");
        fprintf(stderr, "==============\n\n");
        // Simplify, remove duplicates and sort cascades
        for(size_t i = 0; i < cascades.size();) {
            size_t eidx = 0;

            while(eidx < cascades[i].execIDs.size() - 1) {
                if(cascades[i].execIDs[eidx] == cascades[i].execIDs[eidx + 1]) {
                    cascades[i].execIDs.erase(cascades[i].execIDs.begin() + eidx + 1);
                }
                else {
                    eidx++;
                }
            }

            bool sameCascade = false;
            for(size_t j = 0; j < i; j++) {
                // Check for duplicate
                if(cascades[j].causeAddress == cascades[i].causeAddress &&
                   cascades[j].severity == cascades[i].severity) {

                    /* More strict similarity test, disabled because too strict
                   cascades[j].execIDs.size() == cascades[i].execIDs.size()) {

                    for(size_t k = 0; k < cascades[i].execIDs.size(); k++) {
                        if(logEntryLookup(cascades[j].execIDs[k])->address !=
                           logEntryLookup(cascades[i].execIDs[k])->address) {
                            sameCascade = false;
                            break;
                        }
                    }*/
                    sameCascade = true;
                }

                // Remove if duplicate
                if(sameCascade) {
                    cascades[j].similarCascade.push_back(cascades[i].cascadeID);
                    cascades[i] = cascades.back();
                    cascades.pop_back();
                    break;
                }
                // Sort
                else {
                    if(cascades[i].severity > cascades[j].severity) {
                        Cascade tmp = cascades[j];
                        cascades[j] = cascades[i];
                        cascades[i] = tmp;
                    }
                }
            }
            // Conditionnal increment if we didn't suppress cascade i as duplicate
            if(sameCascade == false) {
                i++;
            }
        }
        for(size_t i = 0; i < cascades.size(); i++) {
            fprintf(stderr, "Cascade %" PRIu64 ":\n", cascades[i].cascadeID);
            fprintf(stderr, "--------------------\n\n");
            fprintf(stderr, "%zu other similar cascade encountered\n", cascades[i].similarCascade.size());
            fprintf(stderr, "Cascade length: %zu\n", cascades[i].execIDs.size());

            if(cascades[i].severity == ErrorSeverity::NoImpact) fprintf(stderr, "No Impact classification\n");
            else if(cascades[i].severity == ErrorSeverity::NonCritical) fprintf(stderr, "Non Critical Impact classification\n");
            else if(cascades[i].severity == ErrorSeverity::Critical) fprintf(stderr, "Critical Impact classification\n");

            if(verbosity == LogVerbosity::Summary) {
                fprintf(stderr, "Cause:\n");
                outputLogEntry(this->savedLogs[logEntryLookup(cascades[i].cascadeID)]);
            }
            else if(verbosity >= LogVerbosity::Detail) {
                fprintf(stderr, "Chain:\n");
                for(ssize_t eID : cascades[i].execIDs) {
                    outputLogEntry(this->savedLogs[logEntryLookup(eID)]);
                }
            }
            fprintf(stderr, "\n\n");
        }
    }
}

void ValidatorEngine::logCoverage(const char* filename) {
    FILE *coverageFile = fopen(filename, "w");
    std::vector<std::pair<std::string, uint64_t>> coverageList;

    for(const auto &c : coverage) {
        coverageList.push_back(std::make_pair(c.first, c.second));
    }
    std::sort(coverageList.begin(), coverageList.end(), KVComp<std::string, uint64_t>);
    for(const std::pair<std::string, uint64_t> &c : coverageList) {
        fprintf(coverageFile, "%s: %" PRIu64 "\n", c.first.c_str(), c.second);
    }

    fclose(coverageFile);
}
