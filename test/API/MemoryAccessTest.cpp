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
#include <gtest/gtest.h>
#include "MemoryAccessTest.h"

#include "inttypes.h"

#include "Platform.h"
#include "Memory.hpp"
#include "Range.h"


#define STACK_SIZE 4096
#define FAKE_RET_ADDR 0x666

#define N_SUM(N) ((N)*((N)+1)/2)
#define OFFSET_SUM(N) N_SUM((N) - 1)

struct TestInfo {
    void* buffer;
    size_t buffer_size;
    size_t i;
};

QBDI::rword arrayRead8(volatile char* buffer, size_t size) {
    size_t i = 0, sum = 0;
    for(i = 0; i < size; i++) {
        sum += buffer[i];
    }
    return sum;
}

QBDI::rword arrayRead16(volatile uint16_t* buffer, size_t size) {
    size_t i = 0, sum = 0;
    for(i = 0; i < size; i++) {
        sum += buffer[i];
    }
    return sum;
}

QBDI::rword arrayRead32(volatile uint32_t* buffer, size_t size) {
    size_t i = 0, sum = 0;
    for(i = 0; i < size; i++) {
        sum += buffer[i];
    }
    return sum;
}

QBDI::rword arrayWrite8(volatile uint8_t* buffer, size_t size) {
    size_t i = 0, sum = 0;
    buffer[0] = 0;
    for(i = 1; i < size; i++) {
        buffer[i] = buffer[i-1] + i;
        sum += buffer[i];
    }
    return sum;
}

QBDI::rword arrayWrite16(volatile uint16_t* buffer, size_t size) {
    size_t i = 0, sum = 0;
    buffer[0] = 0;
    for(i = 1; i < size; i++) {
        buffer[i] = buffer[i-1] + i;
        sum += buffer[i];
    }
    return sum;
}

QBDI::rword arrayWrite32(volatile uint32_t* buffer, size_t size) {
    size_t i = 0, sum = 0;
    buffer[0] = 0;
    for(i = 1; i < size; i++) {
        buffer[i] = buffer[i-1] + i;
        sum += buffer[i];
    }
    return sum;
}

QBDI::rword unrolledRead(volatile char* buffer) {
    size_t sum = buffer[0];
    sum += buffer[1];
    sum += buffer[2];
    sum += buffer[3];
    sum += buffer[4];
    sum += buffer[5];
    sum += buffer[6];
    sum += buffer[7];
    sum += buffer[8];
    sum += buffer[9];
    sum += buffer[10];
    return sum;
}

QBDI::rword unrolledWrite(volatile char* buffer) {
    size_t sum = 1;
    buffer[0] = sum;
    sum += buffer[0];
    buffer[1] = sum;
    sum += buffer[1];
    buffer[2] = sum;
    sum += buffer[2];
    buffer[3] = sum;
    sum += buffer[3];
    buffer[4] = sum;
    sum += buffer[4];
    buffer[5] = sum;
    sum += buffer[5];
    buffer[6] = sum;
    sum += buffer[6];
    buffer[7] = sum;
    sum += buffer[7];
    buffer[8] = sum;
    sum += buffer[8];
    buffer[9] = sum;
    sum += buffer[9];
    buffer[10] = sum;
    sum += buffer[10];
    return sum;
}

QBDI::rword mad(volatile uint32_t* a, volatile uint32_t* b, volatile uint32_t* c) {
    uint32_t av = *a;
    uint32_t bv = *b;
    uint32_t cv = *c;
    cv = (av + cv) * (bv + cv);
    *c = cv;
    return *c;
}

void MemoryAccessTest::SetUp() {
    // Constructing a new QBDI vm
    vm = new QBDI::VM();
    ASSERT_NE(vm, nullptr);

    bool instrumented = vm->addInstrumentedModuleFromAddr((QBDI::rword) &arrayRead8);
    ASSERT_TRUE(instrumented);

    // get GPR state
    state = vm->getGPRState();

    // Get a pointer to the GPR state of the vm
    // Setup initial GPR state, this fakestack will produce a ret NULL at the end of the execution
    bool ret = QBDI::allocateVirtualStack(state, STACK_SIZE, &fakestack);
    ASSERT_EQ(ret, true);
}


void MemoryAccessTest::TearDown() {
    QBDI::alignedFree(fakestack);
    delete vm;
}


QBDI::VMAction checkArrayRead8(QBDI::VMInstanceRef vm, QBDI::GPRState* gprState, QBDI::FPRState* fprState, void* data) {

    TestInfo* info = (TestInfo*) data;
    std::vector<QBDI::MemoryAccess> memaccesses = vm->getInstMemoryAccess();
    QBDI::Range<QBDI::rword> brange((QBDI::rword) info->buffer, ((QBDI::rword) info->buffer) + info->buffer_size);
    for(const QBDI::MemoryAccess& memaccess : memaccesses) {
        if(memaccess.type == QBDI::MEMORY_READ) {
            if(brange.contains(memaccess.accessAddress)) {
                size_t offset = memaccess.accessAddress - brange.start;
                if((QBDI::rword) ((uint8_t*)info->buffer)[offset] == memaccess.value) {
                    info->i += offset;
                }
            }
        }
    }
    return QBDI::VMAction::CONTINUE;
}

QBDI::VMAction checkArrayRead16(QBDI::VMInstanceRef vm, QBDI::GPRState* gprState, QBDI::FPRState* fprState, void* data) {

    TestInfo* info = (TestInfo*) data;
    std::vector<QBDI::MemoryAccess> memaccesses = vm->getInstMemoryAccess();
    QBDI::Range<QBDI::rword> brange((QBDI::rword) info->buffer, ((QBDI::rword) info->buffer) + info->buffer_size);
    for(const QBDI::MemoryAccess& memaccess : memaccesses) {
        if(memaccess.type == QBDI::MEMORY_READ) {
            if(brange.contains(memaccess.accessAddress)) {
                size_t offset = (memaccess.accessAddress - brange.start) >> 1;
                if ((QBDI::rword) ((uint16_t*)info->buffer)[offset] == memaccess.value) {
                    info->i += offset;
                }
            }
        }
    }
    return QBDI::VMAction::CONTINUE;
}

QBDI::VMAction checkArrayRead32(QBDI::VMInstanceRef vm, QBDI::GPRState* gprState, QBDI::FPRState* fprState, void* data) {

    TestInfo* info = (TestInfo*) data;
    std::vector<QBDI::MemoryAccess> memaccesses = vm->getInstMemoryAccess();
    QBDI::Range<QBDI::rword> brange((QBDI::rword) info->buffer, ((QBDI::rword) info->buffer) + info->buffer_size);
    for(const QBDI::MemoryAccess& memaccess : memaccesses) {
        if(memaccess.type == QBDI::MEMORY_READ) {
            if(brange.contains(memaccess.accessAddress)) {
                size_t offset = (memaccess.accessAddress - brange.start) >> 2;
                if ((QBDI::rword) ((uint32_t*)info->buffer)[offset] == memaccess.value) {
                    info->i += offset;
                }
            }
        }
    }
    return QBDI::VMAction::CONTINUE;
}

QBDI::VMAction checkArrayWrite8(QBDI::VMInstanceRef vm, QBDI::GPRState* gprState, QBDI::FPRState* fprState, void* data) {

    TestInfo* info = (TestInfo*) data;
    std::vector<QBDI::MemoryAccess> memaccesses = vm->getInstMemoryAccess();
    QBDI::Range<QBDI::rword> brange((QBDI::rword) info->buffer, ((QBDI::rword) info->buffer) + info->buffer_size);
    for(const QBDI::MemoryAccess& memaccess : memaccesses) {
        if(memaccess.type == QBDI::MEMORY_WRITE) {
            if(brange.contains(memaccess.accessAddress)) {
                size_t offset = memaccess.accessAddress - brange.start;
                if ((QBDI::rword) ((uint8_t*)info->buffer)[offset] == memaccess.value) {
                    info->i += offset;
                }
            }
        }
    }
    return QBDI::VMAction::CONTINUE;
}

QBDI::VMAction checkArrayWrite16(QBDI::VMInstanceRef vm, QBDI::GPRState* gprState, QBDI::FPRState* fprState, void* data) {

    TestInfo* info = (TestInfo*) data;
    std::vector<QBDI::MemoryAccess> memaccesses = vm->getInstMemoryAccess();
    QBDI::Range<QBDI::rword> brange((QBDI::rword) info->buffer, ((QBDI::rword) info->buffer) + info->buffer_size);
    for(const QBDI::MemoryAccess& memaccess : memaccesses) {
        if(memaccess.type == QBDI::MEMORY_WRITE) {
            if(brange.contains(memaccess.accessAddress)) {
                size_t offset = (memaccess.accessAddress - brange.start) >> 1;
                if ((QBDI::rword) ((uint16_t*)info->buffer)[offset] == memaccess.value) {
                    info->i += offset;
                }
            }
        }
    }
    return QBDI::VMAction::CONTINUE;
}

QBDI::VMAction checkArrayWrite32(QBDI::VMInstanceRef vm, QBDI::GPRState* gprState, QBDI::FPRState* fprState, void* data) {

    TestInfo* info = (TestInfo*) data;
    std::vector<QBDI::MemoryAccess> memaccesses = vm->getInstMemoryAccess();
    QBDI::Range<QBDI::rword> brange((QBDI::rword) info->buffer, ((QBDI::rword) info->buffer) + info->buffer_size);
    for(const QBDI::MemoryAccess& memaccess : memaccesses) {
        if(memaccess.type == QBDI::MEMORY_WRITE) {
            if(brange.contains(memaccess.accessAddress)) {
                size_t offset = (memaccess.accessAddress - brange.start) >> 2;
                if ((QBDI::rword) ((uint32_t*)info->buffer)[offset] == memaccess.value) {
                    info->i += offset;
                }
            }
        }
    }
    return QBDI::VMAction::CONTINUE;
}

QBDI::VMAction checkUnrolledRead(QBDI::VMInstanceRef vm, const QBDI::VMState* vmState, QBDI::GPRState* gprState, QBDI::FPRState* fprState, void* data) {

    TestInfo* info = (TestInfo*) data;
    std::vector<QBDI::MemoryAccess> memaccesses = vm->getBBMemoryAccess();
    QBDI::Range<QBDI::rword> brange((QBDI::rword) info->buffer, ((QBDI::rword) info->buffer) + info->buffer_size);
    for(const QBDI::MemoryAccess& memaccess : memaccesses) {
        if(memaccess.type == QBDI::MEMORY_READ) {
            if(brange.contains(memaccess.accessAddress)) {
                size_t offset = memaccess.accessAddress - brange.start;
                if((QBDI::rword) ((uint8_t*)info->buffer)[offset] == memaccess.value) {
                    info->i += offset;
                }
            }
        }
    }
    return QBDI::VMAction::CONTINUE;
}

QBDI::VMAction checkUnrolledWrite(QBDI::VMInstanceRef vm, const QBDI::VMState* vmState, QBDI::GPRState* gprState, QBDI::FPRState* fprState, void* data) {

    TestInfo* info = (TestInfo*) data;
    std::vector<QBDI::MemoryAccess> memaccesses = vm->getBBMemoryAccess();
    QBDI::Range<QBDI::rword> brange((QBDI::rword) info->buffer, ((QBDI::rword) info->buffer) + info->buffer_size);
    for(const QBDI::MemoryAccess& memaccess : memaccesses) {
        if(memaccess.type == QBDI::MEMORY_WRITE) {
            if(brange.contains(memaccess.accessAddress)) {
                size_t offset = memaccess.accessAddress - brange.start;
                if((QBDI::rword) ((uint8_t*)info->buffer)[offset] == memaccess.value) {
                    info->i += offset;
                }
            }
        }
    }
    return QBDI::VMAction::CONTINUE;
}

QBDI::VMAction readSnooper(QBDI::VMInstanceRef vm, QBDI::GPRState* gprState, QBDI::FPRState* fprState, void* data) {

    std::vector<QBDI::MemoryAccess> memaccesses = vm->getInstMemoryAccess();
    for(const QBDI::MemoryAccess& memaccess : memaccesses) {
        if(memaccess.type == QBDI::MEMORY_READ) {
            // If read from data
            if(memaccess.accessAddress == (QBDI::rword) data) {
                // replace data
                *((uint32_t*) data) = 0x42;
            }
        }
    }
    return QBDI::VMAction::CONTINUE;
}

QBDI::VMAction writeSnooper(QBDI::VMInstanceRef vm, QBDI::GPRState* gprState, QBDI::FPRState* fprState, void* data) {
    std::vector<QBDI::MemoryAccess> memaccesses = vm->getInstMemoryAccess();
    for(const QBDI::MemoryAccess& memaccess : memaccesses) {
        if(memaccess.type == QBDI::MEMORY_WRITE) {
            // If write to data
            if(memaccess.accessAddress == (QBDI::rword) data) {
                // replace data
                *((uint32_t*) data) = 0x42;
            }
        }
    }
    return QBDI::VMAction::CONTINUE;
}


#if defined(QBDI_ARCH_X86_64)
TEST_F(MemoryAccessTest, Read8) {
#else
TEST_F(MemoryAccessTest, DISABLED_Read8) {
#endif
    char buffer[] = "p0p30fd0p3";
    size_t buffer_size = sizeof(buffer) / sizeof(char);
    TestInfo info = {(void*)buffer, sizeof(buffer), 0};

    vm->addMemAccessCB(QBDI::MEMORY_READ, checkArrayRead8, &info);

    QBDI::simulateCall(state, FAKE_RET_ADDR, {(QBDI::rword) buffer, (QBDI::rword) buffer_size});
    bool ran = vm->run((QBDI::rword) arrayRead8, (QBDI::rword) FAKE_RET_ADDR);

    ASSERT_EQ(true, ran);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) arrayRead8(buffer, buffer_size));
    ASSERT_EQ(OFFSET_SUM(buffer_size), info.i);
}

#if defined(QBDI_ARCH_X86_64)
TEST_F(MemoryAccessTest, Read16) {
#else
TEST_F(MemoryAccessTest, DISABLED_Read16) {
#endif
    uint16_t buffer[] = {44595, 59483, 57377, 31661, 846, 56570, 46925, 62955, 25481, 41095};
    size_t buffer_size = sizeof(buffer) / sizeof(uint16_t);
    TestInfo info = {(void*)buffer, sizeof(buffer), 0};

    vm->addMemAccessCB(QBDI::MEMORY_READ, checkArrayRead16, &info);

    QBDI::simulateCall(state, FAKE_RET_ADDR, {(QBDI::rword) buffer, (QBDI::rword) buffer_size});
    bool ran = vm->run((QBDI::rword) arrayRead16, (QBDI::rword) FAKE_RET_ADDR);

    ASSERT_EQ(true, ran);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) arrayRead16(buffer, buffer_size));
    ASSERT_EQ(OFFSET_SUM(buffer_size), info.i);
}

#if defined(QBDI_ARCH_X86_64)
TEST_F(MemoryAccessTest, Read32) {
#else
TEST_F(MemoryAccessTest, DISABLED_Read32) {
#endif
    uint32_t buffer[] = {3531902336, 1974345459, 1037124602, 2572792182, 3451121073, 4105092976, 2050515100, 2786945221, 1496976643, 515521533};
    size_t buffer_size = sizeof(buffer) / sizeof(uint32_t);
    TestInfo info = {(void*)buffer, sizeof(buffer), 0};

    vm->addMemAccessCB(QBDI::MEMORY_READ, checkArrayRead32, &info);

    QBDI::simulateCall(state, FAKE_RET_ADDR, {(QBDI::rword) buffer, (QBDI::rword) buffer_size});
    bool ran = vm->run((QBDI::rword) arrayRead32, (QBDI::rword) FAKE_RET_ADDR);

    ASSERT_EQ(true, ran);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) arrayRead32(buffer, buffer_size));
    ASSERT_EQ(OFFSET_SUM(buffer_size), info.i);
}

#if defined(QBDI_ARCH_X86_64)
TEST_F(MemoryAccessTest, Write8) {
#else
TEST_F(MemoryAccessTest, DISABLED_Write8) {
#endif
    const size_t buffer_size = 10;
    uint8_t buffer[buffer_size];
    TestInfo info = {(void*)buffer, sizeof(buffer), 0};

    vm->addMemAccessCB(QBDI::MEMORY_WRITE, checkArrayWrite8, &info);

    QBDI::simulateCall(state, FAKE_RET_ADDR, {(QBDI::rword) buffer, (QBDI::rword) buffer_size});
    bool ran = vm->run((QBDI::rword) arrayWrite8, (QBDI::rword) FAKE_RET_ADDR);

    ASSERT_EQ(true, ran);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) arrayWrite8(buffer, buffer_size));
    ASSERT_EQ(OFFSET_SUM(buffer_size), info.i);
}

#if defined(QBDI_ARCH_X86_64)
TEST_F(MemoryAccessTest, Write16) {
#else
TEST_F(MemoryAccessTest, DISABLED_Write16) {
#endif
    const size_t buffer_size = 10;
    uint16_t buffer[buffer_size];
    TestInfo info = {(void*)buffer, sizeof(buffer), 0};

    vm->addMemAccessCB(QBDI::MEMORY_WRITE, checkArrayWrite16, &info);

    QBDI::simulateCall(state, FAKE_RET_ADDR, {(QBDI::rword) buffer, (QBDI::rword) buffer_size});
    bool ran = vm->run((QBDI::rword) arrayWrite16, (QBDI::rword) FAKE_RET_ADDR);

    ASSERT_EQ(true, ran);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) arrayWrite16(buffer, buffer_size));
    ASSERT_EQ(OFFSET_SUM(buffer_size), info.i);
}

#if defined(QBDI_ARCH_X86_64)
TEST_F(MemoryAccessTest, Write32) {
#else
TEST_F(MemoryAccessTest, DISABLED_Write32) {
#endif
    const size_t buffer_size = 10;
    uint32_t buffer[buffer_size];
    TestInfo info = {(void*)buffer, sizeof(buffer), 0};

    vm->addMemAccessCB(QBDI::MEMORY_WRITE, checkArrayWrite32, &info);

    QBDI::simulateCall(state, FAKE_RET_ADDR, {(QBDI::rword) buffer, (QBDI::rword) buffer_size});
    bool ran = vm->run((QBDI::rword) arrayWrite32, (QBDI::rword) FAKE_RET_ADDR);

    ASSERT_EQ(true, ran);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) arrayWrite32(buffer, buffer_size));
    ASSERT_EQ(OFFSET_SUM(buffer_size), info.i);
}

#if defined(QBDI_ARCH_X86_64)
TEST_F(MemoryAccessTest, BasicBlockRead) {
#else
TEST_F(MemoryAccessTest, DISABLED_BasicBlockRead) {
#endif
    char buffer[] = "p0p30fd0p3";
    size_t buffer_size = sizeof(buffer) / sizeof(char);
    TestInfo info = {(void*)buffer, sizeof(buffer), 0};

    vm->recordMemoryAccess(QBDI::MEMORY_READ);
    vm->addVMEventCB(QBDI::VMEvent::BASIC_BLOCK_EXIT, checkUnrolledRead, &info);

    QBDI::simulateCall(state, FAKE_RET_ADDR, {(QBDI::rword) buffer});
    bool ran = vm->run((QBDI::rword) unrolledRead, (QBDI::rword) FAKE_RET_ADDR);

    ASSERT_EQ(true, ran);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) unrolledRead(buffer));
    ASSERT_EQ(OFFSET_SUM(buffer_size), info.i);
}

#if defined(QBDI_ARCH_X86_64)
TEST_F(MemoryAccessTest, BasicBlockWrite) {
#else
TEST_F(MemoryAccessTest, DISABLED_BasicBlockWrite) {
#endif
    const size_t buffer_size = 11;
    char buffer[buffer_size];
    TestInfo info = {(void*)buffer, sizeof(buffer), 0};

    vm->recordMemoryAccess(QBDI::MEMORY_WRITE);
    vm->addVMEventCB(QBDI::VMEvent::BASIC_BLOCK_EXIT, checkUnrolledWrite, &info);

    QBDI::simulateCall(state, FAKE_RET_ADDR, {(QBDI::rword) buffer});
    bool ran = vm->run((QBDI::rword) unrolledWrite, (QBDI::rword) FAKE_RET_ADDR);

    ASSERT_EQ(true, ran);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) unrolledWrite(buffer));
    ASSERT_EQ(OFFSET_SUM(buffer_size), info.i);
}

#if defined(QBDI_ARCH_X86_64)
TEST_F(MemoryAccessTest, ReadRange) {
#else
TEST_F(MemoryAccessTest, DISABLED_ReadRange) {
#endif
    uint32_t buffer[] = {3531902336, 1974345459, 1037124602, 2572792182, 3451121073, 4105092976, 2050515100, 2786945221, 1496976643, 515521533};
    size_t buffer_size = sizeof(buffer) / sizeof(uint32_t);
    TestInfo info = {(void*)buffer, sizeof(buffer), 0};

    vm->addMemRangeCB((QBDI::rword) buffer, (QBDI::rword) (buffer + buffer_size), QBDI::MEMORY_READ, checkArrayRead32, &info);

    QBDI::simulateCall(state, FAKE_RET_ADDR, {(QBDI::rword) buffer, (QBDI::rword) buffer_size});
    bool ran = vm->run((QBDI::rword) arrayRead32, (QBDI::rword) FAKE_RET_ADDR);

    ASSERT_EQ(true, ran);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) arrayRead32(buffer, buffer_size));
    ASSERT_EQ(OFFSET_SUM(buffer_size), info.i);
}

#if defined(QBDI_ARCH_X86_64)
TEST_F(MemoryAccessTest, WriteRange) {
#else
TEST_F(MemoryAccessTest, DISABLED_WriteRange) {
#endif
    const size_t buffer_size = 10;
    uint32_t buffer[buffer_size];
    TestInfo info = {(void*)buffer, sizeof(buffer), 0};

    vm->addMemRangeCB((QBDI::rword) buffer, (QBDI::rword) (buffer + buffer_size), QBDI::MEMORY_WRITE, checkArrayWrite32, &info);

    QBDI::simulateCall(state, FAKE_RET_ADDR, {(QBDI::rword) buffer, (QBDI::rword) buffer_size});
    bool ran = vm->run((QBDI::rword) arrayWrite32, (QBDI::rword) FAKE_RET_ADDR);

    ASSERT_EQ(true, ran);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) arrayWrite32(buffer, buffer_size));
    ASSERT_EQ(OFFSET_SUM(buffer_size), info.i);
}

#if defined(QBDI_ARCH_X86_64)
TEST_F(MemoryAccessTest, ReadWriteRange) {
#else
TEST_F(MemoryAccessTest, DISABLED_ReadWriteRange) {
#endif
    const size_t buffer_size = 10;
    uint32_t buffer[buffer_size];
    TestInfo info = {(void*)buffer, sizeof(buffer), 0};

    // Array write
    uint32_t cb1 = vm->addMemRangeCB((QBDI::rword) buffer, (QBDI::rword) (buffer + buffer_size), QBDI::MEMORY_READ_WRITE, checkArrayWrite32, &info);
    QBDI::simulateCall(state, FAKE_RET_ADDR, {(QBDI::rword) buffer, (QBDI::rword) buffer_size});
    bool ran = vm->run((QBDI::rword) arrayWrite32, (QBDI::rword) FAKE_RET_ADDR);
    ASSERT_EQ(true, ran);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) arrayWrite32(buffer, buffer_size));
    ASSERT_EQ(OFFSET_SUM(buffer_size), info.i);

    // Array read
    info.i = 0;
    vm->deleteInstrumentation(cb1);
    vm->addMemRangeCB((QBDI::rword) buffer, (QBDI::rword) (buffer + buffer_size), QBDI::MEMORY_READ_WRITE, checkArrayRead32, &info);
    QBDI::simulateCall(state, FAKE_RET_ADDR, {(QBDI::rword) buffer, (QBDI::rword) buffer_size});
    ran = vm->run((QBDI::rword) arrayRead32, (QBDI::rword) FAKE_RET_ADDR);
    ASSERT_EQ(true, ran);
    ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) arrayRead32(buffer, buffer_size));
    ASSERT_EQ(OFFSET_SUM(buffer_size), info.i);
}

#if defined(QBDI_ARCH_X86_64)
TEST_F(MemoryAccessTest, MemorySnooping) {
#else
TEST_F(MemoryAccessTest, DISABLED_MemorySnooping) {
#endif
    uint32_t a = 10, b = 42, c = 1337;
    QBDI::rword original = mad(&a, &b, &c);
    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

    // Will replace a with 0x42 on read,  Will replace c with 0x42 on write
    uint32_t snoop1 = vm->addMemAddrCB((QBDI::rword) &a, QBDI::MEMORY_READ, readSnooper, &a);
    uint32_t snoop2 = vm->addMemAddrCB((QBDI::rword) &c, QBDI::MEMORY_WRITE, writeSnooper, &c);
    QBDI::simulateCall(state, FAKE_RET_ADDR, {(QBDI::rword) &a, (QBDI::rword) &b, (QBDI::rword) &c});
    a = 10, b = 42, c = 1337;
    vm->run((QBDI::rword) mad, (QBDI::rword) FAKE_RET_ADDR);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ((QBDI::rword) 0x42, ret);
    // Will replace b with 0x42 on read, no effect because snoop2 is still active
    uint32_t snoop3 = vm->addMemAddrCB((QBDI::rword) &b, QBDI::MEMORY_READ, readSnooper, &b);
    QBDI::simulateCall(state, FAKE_RET_ADDR, {(QBDI::rword) &a, (QBDI::rword) &b, (QBDI::rword) &c});
    a = 10, b = 42, c = 1337;
    vm->run((QBDI::rword) mad, (QBDI::rword) FAKE_RET_ADDR);
    ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ((QBDI::rword) 0x42, ret);
    // Deleting snoop2, effect of snoop1 and snoop3
    vm->deleteInstrumentation(snoop2);
    QBDI::simulateCall(state, FAKE_RET_ADDR, {(QBDI::rword) &a, (QBDI::rword) &b, (QBDI::rword) &c});
    a = 10, b = 42, c = 1337;
    vm->run((QBDI::rword) mad, (QBDI::rword) FAKE_RET_ADDR);
    ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    a = 0x42, b = 0x42, c = 1337;
    ASSERT_EQ(mad(&a, &b, &c), ret);
    // Deleting snoop1, effect of snoop3
    vm->deleteInstrumentation(snoop1);
    QBDI::simulateCall(state, FAKE_RET_ADDR, {(QBDI::rword) &a, (QBDI::rword) &b, (QBDI::rword) &c});
    a = 10, b = 42, c = 1337;
    vm->run((QBDI::rword) mad, (QBDI::rword) FAKE_RET_ADDR);
    ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    a = 10, b = 0x42, c = 1337;
    ASSERT_EQ(mad(&a, &b, &c), ret);
    // Deleting snoop3
    vm->deleteInstrumentation(snoop3);
    QBDI::simulateCall(state, FAKE_RET_ADDR, {(QBDI::rword) &a, (QBDI::rword) &b, (QBDI::rword) &c});
    a = 10, b = 42, c = 1337;
    vm->run((QBDI::rword) mad, (QBDI::rword) FAKE_RET_ADDR);
    ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(original, ret);
}
