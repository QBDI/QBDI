/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2021 Quarkslab
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
#ifndef QBDI_STATE_H_
#define QBDI_STATE_H_

#include "QBDI/Config.h"

#if defined(QBDI_ARCH_X86_64)
#include "QBDI/arch/X86_64/State.h"
#elif defined(QBDI_ARCH_X86)
#include "QBDI/arch/X86/State.h"
#else
#error "No state for this configuration"
#endif

#ifdef __cplusplus

#define QBDI_GPR_GET(state, i) (reinterpret_cast<const QBDI::rword *>(state)[i])

#define QBDI_GPR_SET(state, i, v) \
  (reinterpret_cast<QBDI::rword *>(state)[i] = v)

#else

#define QBDI_GPR_GET(state, i) (((rword *)state)[i])

#define QBDI_GPR_SET(state, i, v) (((rword *)state)[i] = v)

#endif

#endif // QBDI_STATE_H_
