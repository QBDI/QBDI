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
      .value("OPT_BYPASS_PAUTH", Options::OPT_BYPASS_PAUTH,
             "Disable pointeur authentication")
      .value("OPT_ENABLE_BTI", Options::OPT_ENABLE_BTI,
             "Enable BTI on instrumented code")
      .export_values()
      .def_invert()
      .def_repr_str();
}

} // namespace pyQBDI
} // namespace QBDI
