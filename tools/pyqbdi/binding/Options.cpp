/*
 * This file is part of pyQBDI (python binding for QBDI).
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

#include "pyqbdi.hpp"

namespace QBDI {
namespace pyQBDI {

void init_binding_Options(py::module_& m) {

    py::enum_<Options>(m, "Options", "VM options")
        .value("NO_OPT", Options::NO_OPT, "Default value")
        .value("OPT_DISABLE_FPR", Options::OPT_DISABLE_FPR,
                "Disable all operation on FPU (SSE, AVX, SIMD). May break the execution if the target use the FPU.")
        .value("OPT_DISABLE_OPTIONAL_FPR", Options::OPT_DISABLE_OPTIONAL_FPR,
                "Disable context switch optimisation when the target execblock doesn't used FPR")
   #if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
        .value("OPT_ATT_SYNTAX", Options::OPT_ATT_SYNTAX, "Used the AT&T syntax for instruction disassembly")
   #endif
        .export_values()
        .def("__xor__", [](const Options p, const Options other) {return static_cast<Options>(p ^ other);})
        .def("__and__", [](const Options p, const Options other) {return static_cast<Options>(p & other);})
        .def("__or__", [](const Options p, const Options other) {return static_cast<Options>(p | other);});

}

}}

