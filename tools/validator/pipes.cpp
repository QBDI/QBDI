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
#include "pipes.h"
#include <string.h>
#include <stdint.h>

// TODO: Proper error handling

int readCString(char* str, size_t length, FILE* pipe) {
    size_t i = 0;
    while(i < length) {
        if(fread((void*) &str[i], 1, 1, pipe) != 1) {
            return 0;
        }
        if(str[i] == '\0') {
            break;
        }
        i += 1;
    }
    return 1;
}

int writeCString(const char* str, FILE* pipe) {
    size_t len = strlen(str) + 1;
    if(fwrite((void*) str, 1, len, pipe) != len) {
        return 0;
    }
    return 1;
}

int readInstructionEvent(QBDI::rword *address, char *mnemonic, size_t mnemonic_len,
                         char *disassembly, size_t disassembly_len, QBDI::GPRState *gprState,
                         QBDI::FPRState *fprState, FILE* pipe) {
    if(fread((void*) address, sizeof(QBDI::rword), 1, pipe) != 1) {
        return 0;
    }
    if(readCString(mnemonic, mnemonic_len, pipe) != 1) {
        return 0;
    }
    if(readCString(disassembly, disassembly_len, pipe) != 1) {
        return 0;
    }
    if(fread((void*) gprState, sizeof(QBDI::GPRState), 1, pipe) != 1) {
        return 0;
    }
    if(fread((void*) fprState, sizeof(QBDI::FPRState), 1, pipe) != 1) {
        return 0;
    }
    return 1;
}

int writeInstructionEvent(QBDI::rword address, const char* mnemonic, const char* disassembly,
                          QBDI::GPRState *gprState, QBDI::FPRState *fprState, FILE* pipe) {
    if(writeEvent(EVENT::INSTRUCTION, pipe) != 1) {
        return 0;
    }
    if(fwrite((void*) &address, sizeof(QBDI::rword), 1, pipe) != 1) {
        return 0;
    }
    if(writeCString(mnemonic, pipe) != 1) {
        return 0;
    }
    if(writeCString(disassembly, pipe) != 1) {
        return 0;
    }
    if(fwrite((void*) gprState, sizeof(QBDI::GPRState), 1, pipe) != 1) {
        return 0;
    }
    if(fwrite((void*) fprState, sizeof(QBDI::FPRState), 1, pipe) != 1) {
        return 0;
    }
    fflush(pipe);
    return 1;
}

int readMismatchMemAccessEvent(QBDI::rword *address,
                         bool *doRead, bool *mayRead, bool *doWrite, bool *mayWrite,
                         std::vector<QBDI::MemoryAccess>& accesses, FILE* pipe) {
    if(fread((void*) address, sizeof(QBDI::rword), 1, pipe) != 1) {
        return 0;
    }
    unsigned char flags;
    if(fread((void*) &flags, sizeof(unsigned char), 1, pipe) != 1) {
        return 0;
    }
    *doRead = flags & 0x8;
    *mayRead = flags & 0x4;
    *doWrite = flags & 0x2;
    *mayWrite = flags & 0x1;

    QBDI::rword accessSize;
    if(fread((void*) &accessSize, sizeof(QBDI::rword), 1, pipe) != 1) {
        return 0;
    }
    accesses.clear();
    for (unsigned i = 0; i < accessSize; i++) {
        QBDI::MemoryAccess access;
        if(fread((void*) &access, sizeof(QBDI::MemoryAccess), 1, pipe) != 1) {
            return 0;
        }
        accesses.push_back(access);
    }
    return 1;
}

int writeMismatchMemAccessEvent(QBDI::rword address,
                          bool doRead, bool mayRead, bool doWrite, bool mayWrite,
                          const std::vector<QBDI::MemoryAccess>& accesses, FILE* pipe) {
    if(writeEvent(EVENT::MISSMATCHMEMACCESS, pipe) != 1) {
        return 0;
    }
    if(fwrite((void*) &address, sizeof(QBDI::rword), 1, pipe) != 1) {
        return 0;
    }

    unsigned char flags = 0;
    flags |= doRead ? 0x8 : 0x0;
    flags |= mayRead ? 0x4 : 0x0;
    flags |= doWrite ? 0x2 : 0x0;
    flags |= mayWrite ? 0x1 : 0x0;

    if(fwrite((void*) &flags, sizeof(unsigned char), 1, pipe) != 1) {
        return 0;
    }

    QBDI::rword accessSize = accesses.size();
    if(fwrite((void*) &accessSize, sizeof(QBDI::rword), 1, pipe) != 1) {
        return 0;
    }
    for (const auto &access: accesses) {
        if(fwrite((const void*) &access, sizeof(QBDI::MemoryAccess), 1, pipe) != 1) {
            return 0;
        }
    }

    fflush(pipe);
    return 1;
}

int readExecTransferEvent(QBDI::rword* address, FILE* pipe) {
    if(fread((void*) address, sizeof(QBDI::rword), 1, pipe) != 1) {
        return 0;
    }
    return 1;
}

int writeExecTransferEvent(QBDI::rword address, FILE* pipe) {
    if(writeEvent(EVENT::EXEC_TRANSFER, pipe) != 1) {
        return 0;
    }
    if(fwrite((void*) &address, sizeof(QBDI::rword), 1, pipe) != 1) {
        return 0;
    }
    fflush(pipe);
    return 1;
}

int readEvent(EVENT* event, FILE* pipe) {
    if(fread((void*) event, sizeof(EVENT), 1, pipe) != 1) {
        return 0;
    }
    return 1;
}

int writeEvent(EVENT event, FILE* pipe) {
    if(fwrite((void*) &event, sizeof(EVENT), 1, pipe) != 1) {
        return 0;
    }
    fflush(pipe);
    return 1;
}

int readCommand(COMMAND *command, FILE* pipe) {
    if(fread((void*) command, sizeof(COMMAND), 1, pipe) != 1) {
        return 0;
    }
    return 1;
}

int writeCommand(COMMAND command, FILE* pipe) {
    if(fwrite((void*) &command, sizeof(COMMAND), 1, pipe) != 1) {
        return 0;
    }
    fflush(pipe);
    return 1;
}
