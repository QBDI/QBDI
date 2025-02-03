/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2025 Quarkslab
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
#ifndef CONTEXT_H
#define CONTEXT_H

#include "QBDI/Config.h"

namespace QBDI {

struct HostState;

struct Context;

} // namespace QBDI

#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
#include "ExecBlock/X86_64/Context_X86_64.h"
#elif defined(QBDI_ARCH_ARM)
#include "ExecBlock/ARM/Context_ARM.h"
#elif defined(QBDI_ARCH_AARCH64)
#include "ExecBlock/AARCH64/Context_AARCH64.h"
#else
#error "No context for this architecture"
#endif

#endif // CONTEXT_H
