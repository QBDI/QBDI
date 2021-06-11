/*
 * This file is part of pyQBDI (python binding for QBDI).
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

#ifndef PYQBDI_BINDING_HPP
#define PYQBDI_BINDING_HPP

#include <functional>
#include <iomanip>
#include <iostream>
#include <pybind11/functional.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <sstream>
#include "QBDI.h"

#ifdef QBDI_LOG_DEBUG
#define PYQBDI_DEBUG true
#else
#define PYQBDI_DEBUG false
#endif

namespace py = pybind11;
using namespace pybind11::literals;

namespace QBDI {
namespace pyQBDI {

void init_binding_Callback(py::module_ &m);
void init_binding_Errors(py::module_ &m);
void init_binding_InstAnalysis(py::module_ &m);
void init_binding_Logs(py::module_ &m);
void init_binding_Memory(py::module_ &m);
void init_binding_Options(py::module_ &m);
void init_binding_Range(py::module_ &m);
void init_binding_State(py::module_ &m);
void init_binding_VM(py::module_ &m);

void init_utils_Memory(py::module_ &m);
void init_utils_Float(py::module_ &m);

} // namespace pyQBDI
} // namespace QBDI

#endif /* PYQBDI_BINDING_HPP */
