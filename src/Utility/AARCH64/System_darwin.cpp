/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2022 Quarkslab
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

#include <mach/host_info.h>
#include <mach/mach.h>
#include <mach/mach_host.h>

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator.h"
#include "llvm/MC/SubtargetFeature.h"
#include "llvm/Support/Host.h"

#include "QBDI/Config.h"

#include "Utility/LogSys.h"
#include "Utility/System.h"

namespace QBDI {

static const std::string _getHostCPUName() {
  host_basic_info_data_t hostInfo;
  mach_msg_type_number_t infoCount;

  infoCount = HOST_BASIC_INFO_COUNT;
  host_info(mach_host_self(), HOST_BASIC_INFO, (host_info_t)&hostInfo,
            &infoCount);

  // ARM32 kernel
  if (hostInfo.cpu_type == CPU_TYPE_ARM) {
    switch (hostInfo.cpu_subtype) {
      case CPU_SUBTYPE_ARM_V4T:
        return "arm710t";
      case CPU_SUBTYPE_ARM_V6:
        return "arm1136j-s";
      case CPU_SUBTYPE_ARM_V5TEJ:
        return "arm9e";
      case CPU_SUBTYPE_ARM_XSCALE:
        return "xscale";
      case CPU_SUBTYPE_ARM_V7:
        return "cortex-a8";
      case CPU_SUBTYPE_ARM_V7F:
        return "cortex-a9";
      case CPU_SUBTYPE_ARM_V7S:
        return "swift";
      case CPU_SUBTYPE_ARM_V8:
        return "cortex-a53";
      default:
        return "arm710t"; // Minimum model on iOS
    }
  }
#ifdef CPU_TYPE_ARM64
  // ARM64 kernel
  else if (hostInfo.cpu_type == CPU_TYPE_ARM64) {
#if defined(QBDI_ARCH_AARCH64)
    switch (hostInfo.cpu_subtype) {
      case CPU_SUBTYPE_ARM64_ALL:
      case CPU_SUBTYPE_ARM64_V8:
      default:
        return "cyclone";
      case CPU_SUBTYPE_ARM64E:
        return "apple-a12";
    }
#endif
    // armv7 / armv7s on ARM64 kernel
    return "swift";
  }
#endif
  QBDI_WARN("Unknown cpu type, using generic");
  return "generic";
}

const std::string getHostCPUName() {
  static std::once_flag cpuflag;
  static std::string cpuname;

  std::call_once(cpuflag, []() { cpuname = _getHostCPUName(); });

  return cpuname;
}

const std::vector<std::string> getHostCPUFeatures() {
  std::vector<std::string> mattrs = {};
  llvm::StringMap<bool> features;

  bool ret = llvm::sys::getHostCPUFeatures(features);

  if (!ret) {
    features.clear();
    const std::string cpuname = getHostCPUName();
    if (cpuname == "apple-a12" or cpuname == "cyclone") {
      features["fp-armv8"] = true;
      features["fullfp16"] = true;
      features["neon"] = true;
      if (cpuname == "apple-a12") {
        features["v8.3a"] = true;
        features["pauth"] = true;
      }
    } else {
      QBDI_WARN("Fail to detect CPUHostFeatures");
    }
  }

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

  if (ret || fixupFeatures != nullptr) {

    for (const auto &feat : features) {
      QBDI_DEBUG("Feature {}: {}", feat.getKeyData(), feat.getValue());
      if (!feat.getValue()) {
        continue;
      }
      mattrs.push_back(feat.getKey().str());
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
