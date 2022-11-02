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
#include <memory.h>
#include <stdio.h>
#include <vector>

#include <QBDI/Callback.h>
#include <QBDI/State.h>

enum EVENT {
  INSTRUCTION,
  MISSMATCHMEMACCESS,
  EXEC_TRANSFER,
  EXIT,
};

enum COMMAND {
  CONTINUE,
  STOP,
};

int readCString(char *str, size_t length, FILE *pipe);

int writeCString(const char *str, FILE *pipe);

int readInstructionEvent(QBDI::rword *address, char *mnemonic,
                         size_t mnemonic_len, char *disassembly,
                         size_t disassembly_len, QBDI::GPRState *gprState,
                         QBDI::FPRState *fprState, bool *debuggerSkip,
                         FILE *pipe);

int writeInstructionEvent(QBDI::rword address, const char *mnemonic,
                          const char *disassembly, QBDI::GPRState *gprState,
                          QBDI::FPRState *fprState, bool debuggerSkip,
                          FILE *pipe);

int readMismatchMemAccessEvent(QBDI::rword *address, bool *doRead,
                               bool *mayRead, bool *doWrite, bool *mayWrite,
                               std::vector<QBDI::MemoryAccess> &accesses,
                               FILE *pipe);

int writeMismatchMemAccessEvent(QBDI::rword address, bool doRead, bool mayRead,
                                bool doWrite, bool mayWrite,
                                const std::vector<QBDI::MemoryAccess> &accesses,
                                FILE *pipe);

int readExecTransferEvent(QBDI::rword *address, FILE *pipe);

int writeExecTransferEvent(QBDI::rword address, FILE *pipe);

int readEvent(EVENT *event, FILE *pipe);

int writeEvent(EVENT event, FILE *pipe);

int readCommand(COMMAND *command, FILE *pipe);

int writeCommand(COMMAND command, FILE *pipe);
