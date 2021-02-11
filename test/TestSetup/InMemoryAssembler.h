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
#ifndef INMEMORYASSEMBLER_H
#define INMEMORYASSEMBLER_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/Memory.h"

class InMemoryObject {

protected:

    using PF = llvm::sys::Memory::ProtectionFlags;

    llvm::sys::MemoryBlock                   objectBlock;
    llvm::ArrayRef<uint8_t>                  code;

public:

    InMemoryObject(const char* source, const char* cpu = "", const char** mattrs = nullptr);

    InMemoryObject(const InMemoryObject& vm) = delete;
    InMemoryObject(InMemoryObject&& vm) = default;
    InMemoryObject& operator=(const InMemoryObject& vm) = delete;
    InMemoryObject& operator=(const InMemoryObject&& vm) = delete;

    ~InMemoryObject();

    llvm::ArrayRef<uint8_t> getCode() { return code; }
};

#endif
