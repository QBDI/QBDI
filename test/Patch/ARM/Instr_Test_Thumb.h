/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2023 Quarkslab
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
#ifndef INSTR_TEST_THUMB_H
#define INSTR_TEST_THUMB_H

#include <sstream>
#include <string.h>
#include <string>

#include "QBDI/Platform.h"
#include "Patch/Utils.h"

#include "ComparedExecutor_Thumb.h"

class Instr_Test_Thumb : public ComparedExecutor_Thumb {};

QBDI::VMAction increment(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState,
                         QBDI::FPRState *fprState, void *data);

#endif
