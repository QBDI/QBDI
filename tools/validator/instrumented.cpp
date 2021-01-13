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
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <errno.h>
#include <dlfcn.h>
#include <set>
#include <sys/stat.h>

#include "instrumented.h"
#include "pipes.h"

#include <QBDI.h>
#include "Utility/LogSys.h"

#if defined(_QBDI_LOG_DEBUG)
#include <iostream>
#endif

int SAVED_ERRNO = 0;

struct Pipes {
    FILE* ctrlPipe;
    FILE* dataPipe;
};

static QBDI::VMAction step(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
    COMMAND cmd;
    SAVED_ERRNO = errno;
    Pipes *pipes = (Pipes*) data;

    const QBDI::InstAnalysis* instAnalysis = vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION | QBDI::ANALYSIS_DISASSEMBLY);
    // Write a new instruction event
    if(writeInstructionEvent(instAnalysis->address, instAnalysis->mnemonic, instAnalysis->disassembly, gprState, fprState, pipes->dataPipe) != 1) {
        // DATA pipe failure, we exit
        LogError("Validator::Instrumented", "Lost the data pipe, exiting!");
        return QBDI::VMAction::STOP;
    }
    // Read next command
    if(readCommand(&cmd, pipes->ctrlPipe) != 1) {
        // CTRL pipe failure, we exit
        LogError("Validator::Instrumented", "Lost the control pipe, exiting!");
        return QBDI::VMAction::STOP;
    }

    errno = SAVED_ERRNO;
    if(cmd == COMMAND::CONTINUE) {
        // Signaling the VM to continue the execution
        return QBDI::VMAction::CONTINUE;
    }
    else if(cmd == COMMAND::STOP) {
        // Signaling the VM to stop the execution
        return QBDI::VMAction::STOP;
    }
    // FATAL
    LogError("Validator::Instrumented", "Did not recognize command from the control pipe, exiting!");
    return QBDI::VMAction::STOP;
}

static QBDI::VMAction verifyMemoryAccess(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
    SAVED_ERRNO = errno;
    Pipes *pipes = (Pipes*) data;

    const QBDI::InstAnalysis* instAnalysis = vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);

    bool mayRead = instAnalysis->mayLoad_LLVM;
    bool mayWrite = instAnalysis->mayStore_LLVM;

    bool doRead = false;
    bool doWrite = false;
    std::vector<QBDI::MemoryAccess> accesses = vm->getInstMemoryAccess();
    for (auto &m : accesses) {
        if ((m.type & QBDI::MEMORY_READ) != 0) {
            doRead = true;
        }
        if ((m.type & QBDI::MEMORY_WRITE) != 0) {
            doWrite = true;
        }
    }

    // llvm API mayRead and mayWrite are incomplete.
    bool bypassRead = false;
    bool bypassWrite = false;

    if (doRead and !mayRead) {
#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
        // all return instructions read the return address.
        bypassRead |= instAnalysis->isReturn;
        const std::set<std::string> shouldReadInsts {
            "CMPSB", "CMPSW", "CMPSL", "CMPSQ",
            "MOVSB", "MOVSW", "MOVSL", "MOVSQ",
            "SCASB", "SCASW", "SCASL", "SCASQ",
        };
        bypassRead |= (shouldReadInsts.count(instAnalysis->mnemonic) == 1);
#endif
    } else if (!doRead and mayRead) {
#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
        const std::set<std::string> noReadInsts {
            "VZEROUPPER", "VZEROALL",
        };
        bypassRead |= (noReadInsts.count(instAnalysis->mnemonic) == 1);
#endif
    }

    if (doWrite and !mayWrite) {
#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
        // all call instructions write the return address.
        bypassWrite |= instAnalysis->isCall;
        const std::set<std::string> shouldWriteInsts {
            "STOSB", "STOSW", "STOSL", "STOSQ",
            "MOVSB", "MOVSW", "MOVSL", "MOVSQ",
        };
        bypassWrite |= (shouldWriteInsts.count(instAnalysis->mnemonic) == 1);
#endif
    } else if (!doWrite and mayWrite) {
#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
        const std::set<std::string> noWriteInsts {
            "VZEROUPPER", "VZEROALL",
        };
        bypassWrite |= (noWriteInsts.count(instAnalysis->mnemonic) == 1);
#endif
    }


    if ((doRead == mayRead || bypassRead) && (doWrite == mayWrite || bypassWrite)) {
        errno = SAVED_ERRNO;
        return QBDI::VMAction::CONTINUE;
    }


    // Write a new instruction event
    if(writeMismatchMemAccessEvent(instAnalysis->address, doRead, instAnalysis->mayLoad,
                                   doWrite, instAnalysis->mayStore, accesses, pipes->dataPipe) != 1) {
        // DATA pipe failure, we exit
        LogError("Validator::Instrumented", "Lost the data pipe, exiting!");
        return QBDI::VMAction::STOP;
    }
    errno = SAVED_ERRNO;
    // Continue the execution
    return QBDI::VMAction::CONTINUE;
}

#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
static QBDI::VMAction logSyscall(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
    Pipes *pipes = (Pipes*) data;
    // We don't have the address, it just need to be different from 0
    writeExecTransferEvent(1, pipes->dataPipe);
    return QBDI::VMAction::CONTINUE;
}
#endif

static QBDI::VMAction logTransfer(QBDI::VMInstanceRef vm, const QBDI::VMState *state, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
    Pipes *pipes = (Pipes*) data;
    writeExecTransferEvent(state->basicBlockStart, pipes->dataPipe);
    return QBDI::VMAction::CONTINUE;
}

static QBDI::VMAction saveErrno(QBDI::VMInstanceRef vm, const QBDI::VMState *state, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
    SAVED_ERRNO = errno;
    return QBDI::VMAction::CONTINUE;
}

static QBDI::VMAction restoreErrno(QBDI::VMInstanceRef vm, const QBDI::VMState *state, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
    errno = SAVED_ERRNO;
    return QBDI::VMAction::CONTINUE;
}

Pipes PIPES = {NULL, NULL};
QBDI::VM* VM;

void cleanup_instrumentation() {
    static bool cleaned_up = false;
    if(cleaned_up == false) {
        writeEvent(EVENT::EXIT, PIPES.dataPipe);
        fclose(PIPES.ctrlPipe);
        fclose(PIPES.dataPipe);
        delete VM;
        cleaned_up = true;
    }
}

void start_instrumented(QBDI::VM* vm, QBDI::rword start, QBDI::rword stop, int ctrlfd, int datafd) {

    VM = vm;
    QBDI::LOGSYS.addFilter("*", QBDI::LogPriority::ERROR);
#if defined(_QBDI_LOG_DEBUG)
    for(const QBDI::MemoryMap& m :  QBDI::getCurrentProcessMaps()) {
        std::cout << m.name << " (" << m.permission << ") ";
        m.range.display(std::cout);
        std::cout << std::endl;
    }
#endif
    // Opening communication FIFO
    PIPES.ctrlPipe = fdopen(ctrlfd, "rb");
    PIPES.dataPipe = fdopen(datafd, "wb");
    if(PIPES.ctrlPipe == nullptr || PIPES.dataPipe == nullptr) {
        LogError("Validator::Instrumented", "Could not open communication pipes with master, exiting!");
        return;
    }

    vm->addCodeCB(QBDI::PREINST, step, (void*) &PIPES);
#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
    // memory Access are not supported for ARM now
    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addCodeCB(QBDI::POSTINST, verifyMemoryAccess, (void*) &PIPES);

    vm->addMnemonicCB("syscall", QBDI::POSTINST, logSyscall, (void*) &PIPES);
#endif
    vm->addVMEventCB(QBDI::VMEvent::EXEC_TRANSFER_CALL, logTransfer, (void*) &PIPES);
    vm->addVMEventCB(QBDI::VMEvent::EXEC_TRANSFER_CALL | QBDI::VMEvent::BASIC_BLOCK_ENTRY, restoreErrno, nullptr);
    vm->addVMEventCB(QBDI::VMEvent::EXEC_TRANSFER_RETURN | QBDI::VMEvent::BASIC_BLOCK_EXIT, saveErrno, nullptr);

    vm->run(start, stop);

    cleanup_instrumentation();
}
