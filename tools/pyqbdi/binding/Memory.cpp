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

#include "Enum.hpp"
#include "pyqbdi.hpp"

namespace QBDI {
namespace pyQBDI {

void init_binding_Memory(py::module_ &m) {

  enum_int_flag_<Permission>(m, "Permission", "Memory access rights.",
                             py::arithmetic())
      .value("PF_NONE", Permission::PF_NONE, "No access")
      .value("PF_READ", Permission::PF_READ, "Read access")
      .value("PF_WRITE", Permission::PF_WRITE, "Write access")
      .value("PF_EXEC", Permission::PF_EXEC, "Execution access")
      .export_values()
      .def_invert()
      .def_repr_str();

  py::class_<MemoryMap>(m, "MemoryMap")
      .def_readwrite("range", &MemoryMap::range,
                     "A range of memory (region), delimited between a start "
                     "and an (excluded) end address.")
      .def_property(
          "permission", [](const MemoryMap &map) { return map.permission; },
          [](MemoryMap &map, int permission) {
            map.permission = static_cast<Permission>(permission);
          },
          "Region access rights (PF_READ, PF_WRITE, PF_EXEC).")
      .def_readwrite("name", &MemoryMap::name,
                     "Region name (useful when a region is mapping a module).");

  m.def("getRemoteProcessMaps", &getRemoteProcessMaps,
        "Get a list of all the memory maps (regions) of a process.", "pid"_a,
        "full_path"_a = false);

  m.def("getCurrentProcessMaps", &getCurrentProcessMaps,
        "Get a list of all the memory maps (regions) of the current process.",
        "full_path"_a = false);

  m.def("getModuleNames",
        static_cast<std::vector<std::string> (*)()>(&getModuleNames),
        "Get a list of all the module names loaded in the process memory.");

  m.def(
      "alignedAlloc",
      [](size_t size, size_t align) {
        return reinterpret_cast<rword>(alignedAlloc(size, align));
      },
      "Allocate a block of memory of a specified sized with an aligned base "
      "address.",
      "size"_a, "align"_a);

  m.def(
      "alignedFree",
      [](rword ptr) { alignedFree(reinterpret_cast<void *>(ptr)); },
      "Free a block of aligned memory allocated with alignedAlloc.", "ptr"_a);

  m.def(
      "allocateVirtualStack",
      [](GPRState *gprstate, uint32_t size) {
        uint8_t *a = NULL;
        if (allocateVirtualStack(gprstate, size, &a)) {
          return static_cast<py::object>(py::int_(reinterpret_cast<rword>(a)));
        } else {
          return static_cast<py::object>(py::none());
        }
      },
      "Allocate a new stack and setup the GPRState accordingly.\n"
      "The allocated stack needs to be freed with alignedFree().\n"
      "The result was int, or None if the allocation fails.",
      "gprstate"_a, "size"_a);

  m.def("simulateCall", &simulateCall,
        "Simulate a call by modifying the stack and registers accordingly.",
        "ctx"_a, "returnAddress"_a, "args"_a = std::vector<rword>());
}

} // namespace pyQBDI
} // namespace QBDI
