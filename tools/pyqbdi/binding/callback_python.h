/*
 * This file is part of pyQBDI.
 *
 * Copyright 2017 - 2025 Quarkslab
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
#ifndef CALLBACK_PYTHON_H_
#define CALLBACK_PYTHON_H_

#include "pyqbdi.hpp"

namespace QBDI {
namespace pyQBDI {

// Python Callback
using PyInstCallback = std::function<VMAction(VMInstanceRef, GPRState *,
                                              FPRState *, py::object &)>;
using PyVMCallback = std::function<VMAction(
    VMInstanceRef, const VMState *, GPRState *, FPRState *, py::object &)>;

struct InstrRuleDataCBKPython {
  PyInstCallback cbk;
  py::object data;
  InstPosition position;
  int priority;

  InstrRuleDataCBKPython(PyInstCallback &cbk, py::object &data,
                         InstPosition position, int priority = PRIORITY_DEFAULT)
      : cbk(cbk), data(data), position(position), priority(priority) {}
};

using PyInstrRuleCallback = std::function<std::vector<InstrRuleDataCBKPython>(
    VMInstanceRef, const InstAnalysis *, py::object &)>;

} // namespace pyQBDI
} // namespace QBDI

#endif /* CALLBACK_PYTHON_H_ */
