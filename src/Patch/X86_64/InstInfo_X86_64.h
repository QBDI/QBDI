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
#include <stdint.h>
#include "Patch/Types.h"
#include "Patch/InstInfo.h"

#ifndef INSTINFO_X86_64_H
#define INSTINFO_X86_64_H

namespace QBDI {

/* Highest 8 bits are the write access, lowest 8 bits are the read access. For each 8 bits part: the
 * highest bit stores if the access is a stack access or not while the lowest 7 bits store the
 * unsigned access size in bytes (thus up to 127 bytes). A size of 0 means no access.
 *
 * -----------------------------------------------------------------------------------------------------------------
 * |                     WRITE ACCESS                      |                      READ ACCESS                      |
 * -----------------------------------------------------------------------------------------------------------------
 * | 1 bit stack access flag | 7 bits unsigned access size | 1 bit stack access flag | 7 bits unsigned access size |
 * -----------------------------------------------------------------------------------------------------------------
*/

#define STACK_ACCESS_FLAG 0x40
#define READ(s) (s)
#define WRITE(s) ((s)<<8)
#define STACK_READ(s) (STACK_ACCESS_FLAG | s)
#define STACK_WRITE(s) ((STACK_ACCESS_FLAG | s)<<8)
#define GET_READ_SIZE(v) ((v) & 0x3f)
#define GET_WRITE_SIZE(v) (((v)>>8) & 0x3f)
#define IS_STACK_READ(v) (((v) & STACK_ACCESS_FLAG) > 0)
#define IS_STACK_WRITE(v) ((((v)>>8) & STACK_ACCESS_FLAG) > 0)

extern uint16_t MEMACCESS_INFO_TABLE[];

};

#endif // INSTINFO_X86_64_H
