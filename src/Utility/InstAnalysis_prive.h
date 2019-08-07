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
#ifndef INSTANALYSISPRIVE_H
#define INSTANALYSISPRIVE_H

#include <map>
#include <vector>

#include "InstAnalysis.h"
#include "Patch/PatchRule.h"
#include "Utility/Assembly.h"


namespace QBDI {

void freeInstAnalysis(InstAnalysis* analysis);

InstAnalysis* analyzeInstMetadataUncached(const InstMetadata* instMetadata, AnalysisType type,
                                          llvm::MCInstrInfo& MCII, llvm::MCRegisterInfo& MRI,
                                          Assembly& assembly);


}

#endif
