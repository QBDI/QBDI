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

#include "Patch/InstrRules.h"
#include "Patch/PatchGenerator.h"

namespace QBDI {

/*! Output a list of PatchGenerator which would set up the host state part of the context for
 *  a callback.
 *
 * @param[in] cbk   The callback function to call.
 * @param[in] data  The data to pass as an argument to the callback function.
 *
 * @return A list of PatchGenerator to set up this callback call.
 *
*/
PatchGenerator::SharedPtrVec getCallbackGenerator(InstCallback cbk, void* data) {
    PatchGenerator::SharedPtrVec callbackGenerator;

    // Write callback address in host state
    callbackGenerator.push_back(GetConstant(Temp(0), Constant((rword) cbk)));
    callbackGenerator.push_back(WriteTemp(Temp(0), Offset(offsetof(Context, hostState.callback))));
    // Write callback data pointer in host state
    callbackGenerator.push_back(GetConstant(Temp(0), Constant((rword) data)));
    callbackGenerator.push_back(WriteTemp(Temp(0), Offset(offsetof(Context, hostState.data))));
    // Write internal instruction id of a callback
    callbackGenerator.push_back(GetInstId(Temp(0)));
    callbackGenerator.push_back(WriteTemp(Temp(0), Offset(offsetof(Context, hostState.origin))));

    return callbackGenerator;
}

}
