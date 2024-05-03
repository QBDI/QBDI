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
#ifndef PATCH_TEST_H
#define PATCH_TEST_H

#include <sstream>
#include <string.h>
#include <string>

#include "QBDI/Platform.h"
#include "Patch/Utils.h"

#if defined(QBDI_ARCH_X86)
#include "X86/ComparedExecutor_X86.h"

class Patch_Test : public ComparedExecutor_X86 {};
#elif defined(QBDI_ARCH_X86_64)
#include "X86_64/ComparedExecutor_X86_64.h"

class Patch_Test : public ComparedExecutor_X86_64 {};
#elif defined(QBDI_ARCH_ARM)
#include "ARM/ComparedExecutor_ARM.h"

class Patch_Test : public ComparedExecutor_ARM {};

#elif defined(QBDI_ARCH_AARCH64)
#include "AARCH64/ComparedExecutor_AARCH64.h"

class Patch_Test : public ComparedExecutor_AARCH64 {};

#else
#error "Architecture not supported"
#endif

#endif
