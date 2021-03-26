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
#ifndef INSTR_X86_64TEST_H
#define INSTR_X86_64TEST_H

#include <sstream>
#include <string.h>
#include <string>

#include "Platform.h"

#if defined(QBDI_ARCH_X86)
#include "ComparedExecutor_X86.h"

class Instr_X86_64Test : public ComparedExecutor_X86 {};
#else
#include "ComparedExecutor_X86_64.h"

class Instr_X86_64Test : public ComparedExecutor_X86_64 {};
#endif

#endif
