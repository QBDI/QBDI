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

/* Highest 16 bits are the write access, lowest 16 bits are the read access. For each 16 bits part: the
 * highest bit stores if the access is a stack access or not while the lowest 12 bits store the
 * unsigned access size in bytes (thus up to 4095 bytes). A size of 0 means no access.
 *
 * ----------------------------------------------------------------------------
 * | Ox1f                        WRITE ACCESS                            0x10 |
 * ----------------------------------------------------------------------------
 * | 1 bit stack access flag | 3 bits reserved | 12 bits unsigned access size |
 * ----------------------------------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------
 * | 0xf                                   READ ACCESS                                        0x0 |
 * ------------------------------------------------------------------------------------------------
 * | 1 bit stack access flag | 1 bit double read | 2 bits reserved | 12 bits unsigned access size |
 * ------------------------------------------------------------------------------------------------
*/

#define STACK_ACCESS_FLAG 0x8000
#define DOUBLE_READ_FLAG  0x4000
#define READ(s) (s)
#define WRITE(s) ((s)<<16)
#define STACK_READ(s) (STACK_ACCESS_FLAG | s)
#define STACK_WRITE(s) ((STACK_ACCESS_FLAG | s)<<16)
#define GET_READ_SIZE(v) ((v) & 0xfff)
#define GET_WRITE_SIZE(v) (((v)>>16) & 0xfff)
#define IS_STACK_READ(v) (((v) & STACK_ACCESS_FLAG) > 0)
#define IS_STACK_WRITE(v) ((((v)>>16) & STACK_ACCESS_FLAG) > 0)
#define IS_DOUBLE_READ(v) (((v) & DOUBLE_READ_FLAG) > 0)

extern uint32_t MEMACCESS_INFO_TABLE[];

};

#endif // INSTINFO_X86_64_H
