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
#ifndef QBDI_OPTION_X86_64_H_
#define QBDI_OPTION_X86_64_H_

#include <stdint.h>

#include "QBDI/Bitmask.h"
#include "QBDI/Platform.h"

#ifdef __cplusplus
namespace QBDI {
#endif

typedef enum {
  _QBDI_EI(NO_OPT) = 0, /*!< Default value */
  // general options between 0 and 23
  _QBDI_EI(OPT_DISABLE_FPR) = 1 << 0,          /*!< Disable all operation on FPU
                                                * (SSE, AVX, SIMD). May break
                                                * the execution if the target
                                                * use the FPU
                                                */
  _QBDI_EI(OPT_DISABLE_OPTIONAL_FPR) = 1 << 1, /*!< Disable context switch
                                                * optimisation when the target
                                                * execblock doesn't used FPR
                                                */
  _QBDI_EI(OPT_DISABLE_MEMORYACCESS_VALUE) = 1 << 2, /*!< Don't load the value
                                                      * when perform memory
                                                      * access.
                                                      */
  // architecture specific option between 24 and 31
  _QBDI_EI(OPT_ATT_SYNTAX) = 1 << 24,   /*!< Used the AT&T syntax for
                                         * instruction disassembly
                                         */
  _QBDI_EI(OPT_ENABLE_FS_GS) = 1 << 25, /*!< Enable Backup/Restore of FS/GS
                                         * segment. This option uses the
                                         * instructions (RD|WR)(FS|GS)BASE that
                                         * must be supported by the operating
                                         * system */
} Options;

_QBDI_ENABLE_BITMASK_OPERATORS(Options)

#ifdef __cplusplus
}
#endif

#endif /* QBDI_OPTION_X86_64_H_ */
