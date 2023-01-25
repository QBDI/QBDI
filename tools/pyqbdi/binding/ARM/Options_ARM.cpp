/*
 * This file is part of pyQBDI (python binding for QBDI).
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

#include "Enum.hpp"
#include "pyqbdi.hpp"

namespace QBDI {
namespace pyQBDI {

void init_binding_Options(py::module_ &m) {

  enum_int_flag_<Options>(m, "Options", "VM options", py::arithmetic())
      .value("NO_OPT", Options::NO_OPT, "Default value")
      .value("OPT_DISABLE_FPR", Options::OPT_DISABLE_FPR,
             "Disable all operation on FPU (SSE, AVX, SIMD). May break the "
             "execution if the target use the FPU.")
      .value("OPT_DISABLE_OPTIONAL_FPR", Options::OPT_DISABLE_OPTIONAL_FPR,
             "Disable context switch optimisation when the target execblock "
             "doesn't used FPR")
      .value("OPT_DISABLE_LOCAL_MONITOR", Options::OPT_DISABLE_LOCAL_MONITOR,
             "Disable the local monitor for instruction like stxr")
      .value("OPT_DISABLE_D16_D31", Options::OPT_DISABLE_D16_D31,
             "Disable the used of D16-D31 register")
      .value("OPT_ARMv4", Options::OPT_ARMv4,
             "Change between ARM and Thumb as an ARMv4 CPU")
      .value("OPT_ARMv5T_6", Options::OPT_ARMv5T_6,
             "Change between ARM and Thumb as an ARMv5T or ARMv6 CPU")
      .value("OPT_ARMv7", Options::OPT_ARMv7,
             "Change between ARM and Thumb as an ARMv7 CPU (default)")
      .value("OPT_ARM_MASK", Options::OPT_ARM_MASK, "")
      .export_values()
      .def_invert()
      .def_repr_str();
}

} // namespace pyQBDI
} // namespace QBDI
