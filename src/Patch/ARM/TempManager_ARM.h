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
#ifndef TempManager_ARM_H
#define TempManager_ARM_H

#include "Patch/TempManager.h"
#include "Patch/Types.h"

namespace QBDI {

// allows to select 2 temp register where the selected register is
// reg1 + 1 == reg2 and with reg1 % 2 in [r0,r2,r4,r6,r8,r10]
void allocateConsecutiveTempRegister(TempManager &temp_manager, Temp temp1,
                                     Temp temp2);

} // namespace QBDI

#endif
