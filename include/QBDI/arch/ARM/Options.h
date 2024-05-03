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
#ifndef QBDI_OPTION_ARM_H_
#define QBDI_OPTION_ARM_H_

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
  // architecture specific option between 24 and 31
  _QBDI_EI(OPT_DISABLE_LOCAL_MONITOR) =
      1 << 24, /*!< Disable the local monitor for instruction like strex */
  _QBDI_EI(OPT_DISABLE_D16_D31) = 1 << 25, /*!< Disable the used of D16-D31
                                            * register
                                            */
  _QBDI_EI(OPT_ARMv4_bit) = 1 << 26,
  _QBDI_EI(OPT_ARMv4) = 3 << 26,    /*!< Change between ARM and Thumb as
                                     * an ARMv4 CPU
                                     */
  _QBDI_EI(OPT_ARMv5T_6) = 1 << 27, /*!< Change between ARM and Thumb as
                                     * an ARMv5T or ARMv6 CPU
                                     */
  _QBDI_EI(OPT_ARMv7) = 0,          /*!< Change between ARM and Thumb as
                                     * an ARMv7 CPU (default)
                                     */
  _QBDI_EI(OPT_ARM_MASK) = 3 << 26,

} Options;

_QBDI_ENABLE_BITMASK_OPERATORS(Options)

#ifdef __cplusplus
}
#endif

#endif /* QBDI_OPTION_ARM_H_ */
