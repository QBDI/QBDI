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

/* pyQBDI module */

namespace QBDI {
namespace pyQBDI {

PYBIND11_MODULE(pyqbdi, m) {
    m.attr("__version__") = QBDI_VERSION_STRING;
    m.attr("__debug__") = PYQBDI_DEBUG;

    init_utils_Float(m);
    init_utils_Memory(m);

}

}}
