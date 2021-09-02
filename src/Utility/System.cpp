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
#include <algorithm>
#include <stdlib.h>

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator.h"
#include "llvm/MC/SubtargetFeature.h"
#include "llvm/Support/Host.h"

#include "QBDI/Config.h"

#include "Utility/System.h"

namespace QBDI {

const std::vector<std::string> getHostCPUFeatures() {
  std::vector<std::string> mattrs = {};
  llvm::StringMap<bool> features;

  bool ret = llvm::sys::getHostCPUFeatures(features);

  if (ret) {
    const char *fixupFeatures = getenv("QBDI_FIXUP_FEATURES");
    if (fixupFeatures != nullptr) {
      llvm::SubtargetFeatures addFeatures(fixupFeatures);
      for (const auto &f : addFeatures.getFeatures()) {
        if (llvm::SubtargetFeatures::hasFlag(f)) {
          features[llvm::SubtargetFeatures::StripFlag(f)] =
              llvm::SubtargetFeatures::isEnabled(f);
        } else {
          features[f] = true;
        }
      }
    }

#if defined(_QBDI_FORCE_DISABLE_AVX)
    const char *disable_avx = "1";
#else
    const char *disable_avx = getenv("QBDI_FORCE_DISABLE_AVX");
#endif

#if defined(_QBDI_ASAN_ENABLED_)
    const char *asan_blacklist_feature[] = {
#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
      "x87",
      "fxsr",
      "xsave",
      "xsaveopt",
      "xsavec",
      "xsaves",
      "sse",
      "sse2",
      "sse3",
      "ssse3",
      "sse4.1",
      "sse4.2",
      "mmx",
      "3dnow",
      "3dnowa",
      "sse4a",
      "avx",
      "avx2",
      "fma",
      "f16c",
      "pclmul",
      "gfni",
      "vpclmulqdq",
      "fma4",
      "xop",
      "aes",
      "vaes",
      "sha"
#endif
    };
    const size_t asan_blacklist_feature_size =
        sizeof(asan_blacklist_feature) / sizeof(char *);
#endif

    for (const auto &feat : features) {
      if (!feat.second) {
        continue;
      }
      // XXX: #19 Bad AVX support detection in VM environments
      // fix buggy dynamic detection
      if (disable_avx != NULL && feat.first().equals(llvm::StringRef("avx"))) {
        continue;
      }
#if defined(_QBDI_ASAN_ENABLED_)
      bool blacklist_feature = false;
      for (int i = 0; i < asan_blacklist_feature_size; i++) {
        if (feat.first().equals(llvm::StringRef(asan_blacklist_feature[i]))) {
          blacklist_feature = true;
          break;
        }
      }
      if (blacklist_feature) {
        continue;
      }
#endif
      mattrs.push_back(feat.first());
    }
  }

  if constexpr (is_arm)
    // set default ARM CPU
    if (features.size() == 0) {
      features.insert({"fp16", true});
      features.insert({"d16", true});
    }
  // Fixing awfull LLVM API
  if (features.count("fp16") && features["fp16"]) {
    mattrs.emplace_back("vfp2");
  }
  if (features.count("d16") && features["d16"]) {
    mattrs.emplace_back("vfp3");
  }
  return mattrs;
}

bool isHostCPUFeaturePresent(const char *query) {
  std::vector<std::string> features = getHostCPUFeatures();
  for (const std::string &feature : features) {
    if (feature == query) {
      return true;
    }
  }
  return false;
}

} // namespace QBDI
