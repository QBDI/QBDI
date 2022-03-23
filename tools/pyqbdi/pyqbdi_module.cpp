/*
 * This file is part of pyQBDI (python binding for QBDI).
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

#include "pyqbdi.hpp"

/* pyQBDI module */

namespace QBDI {
namespace pyQBDI {

PYBIND11_MODULE(pyqbdi, m) {
  m.doc() = "python binding for QBDI";
  m.attr("__version__") = getVersion(nullptr);
  m.attr("__debug__") = PYQBDI_DEBUG;
  m.attr("__arch__") = QBDI_ARCHITECTURE_STRING;
  m.attr("__platform__") = QBDI_PLATFORM_STRING;
  m.attr("__preload__") = false;

  init_binding_Range(m);
  init_binding_State(m);
  init_binding_Options(m);
  init_binding_Memory(m);
  init_binding_InstAnalysis(m);
  init_binding_Callback(m);
  init_binding_VM(m);
  init_binding_Logs(m);
  init_binding_Errors(m);

  init_utils_Float(m);
  init_utils_Memory(m);
}

} // namespace pyQBDI
} // namespace QBDI
