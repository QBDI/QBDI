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
#include "Platform.h"

#include "llvm/Support/Host.h"
#include "llvm/Support/Process.h"

#include "Utility/LogSys.h"
#include "System.h"


namespace QBDI {

const std::vector<std::string> getHostCPUFeatures() {
    std::vector<std::string> mattrs = {};
    llvm::StringMap<bool> features;

    bool ret = llvm::sys::getHostCPUFeatures(features);
    if (ret) {
        for (const auto& feat: features) {
            // XXX: #19 Bad AVX support detection in VM environments
            #if defined(_QBDI_FORCE_DISABLE_AVX)
            char* disable_avx = "1";
            #else
            char* disable_avx = getenv("QBDI_FORCE_DISABLE_AVX");
            #endif
            // fix buggy dynamic detection
            if(disable_avx != NULL &&
                    feat.first().equals(llvm::StringRef("avx"))) {
                continue;
            }
            if(feat.second) {
                mattrs.push_back(feat.first());
            }
        }
    }
        
    #if defined(QBDI_ARCH_ARM)
	// set default ARM CPU
	if(features.size() == 0) {
		features.insert({"fp16", true});
		features.insert({"d16", true});
	}
    #endif
    // Fixing awfull LLVM API
    if(features.count("fp16") && features["fp16"]) {
        mattrs.push_back(std::string("vfp2"));
    }
    if(features.count("d16") && features["d16"]) {
        mattrs.push_back(std::string("vfp3"));
    }
    return mattrs; 
}


bool isHostCPUFeaturePresent(const char* query) {
   std::vector<std::string> features = getHostCPUFeatures();
   for(const std::string& feature: features) {
        if(feature == query) {
            return true;
        }
   }
   return false;
}

}
