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

void init_binding_Callback(py::module& m) {

    py::enum_<VMAction>(m, "VMAction", "The callback results.")
        .value("CONTINUE", VMAction::CONTINUE, "The execution of the basic block continues.")
        .value("BREAK_TO_VM", VMAction::BREAK_TO_VM,
                "The execution breaks and returns to the VM causing a complete reevaluation of "
                "the execution state. A BREAK_TO_VM is needed to ensure that modifications of "
                "the Program Counter or the program code are taken into account.")
        .value("STOP", VMAction::STOP,
                "Stops the execution of the program. This causes the run function to return early.")
        .export_values();

    py::enum_<InstPosition>(m, "InstPosition", "Position relative to an instruction.")
        .value("PREINST", InstPosition::PREINST, "Positioned before the instruction.")
        .value("POSTINST", InstPosition::POSTINST, "Positioned after the instruction.")
        .export_values();

    py::enum_<VMEvent>(m, "VMEvent", py::arithmetic())
        .value("SEQUENCE_ENTRY", VMEvent::SEQUENCE_ENTRY, "Triggered when the execution enters a sequence.")
        .value("SEQUENCE_EXIT", VMEvent::SEQUENCE_EXIT, "Triggered when the execution exits from the current sequence.")
        .value("BASIC_BLOCK_ENTRY", VMEvent::BASIC_BLOCK_ENTRY, "Triggered when the execution enters a basic block.")
        .value("BASIC_BLOCK_EXIT", VMEvent::BASIC_BLOCK_EXIT, "Triggered when the execution exits from the current basic block.")
        .value("BASIC_BLOCK_NEW", VMEvent::BASIC_BLOCK_NEW, "Triggered when the execution enters a new (~unknown) basic block.")
        .value("EXEC_TRANSFER_CALL", VMEvent::EXEC_TRANSFER_CALL, "Triggered when the ExecBroker executes an execution transfer.")
        .value("EXEC_TRANSFER_RETURN", VMEvent::EXEC_TRANSFER_RETURN, "Triggered when the ExecBroker returns from an execution transfer.")
        .export_values()
        .def("__str__",
                [](const VMEvent p) {
                    std::string res;
                    if (p & VMEvent::SEQUENCE_ENTRY)
                        res += "|VMEvent.SEQUENCE_ENTRY";
                    if (p & VMEvent::SEQUENCE_EXIT)
                        res += "|VMEvent.SEQUENCE_EXIT";
                    if (p & VMEvent::BASIC_BLOCK_ENTRY)
                        res += "|VMEvent.BASIC_BLOCK_ENTRY";
                    if (p & VMEvent::BASIC_BLOCK_EXIT)
                        res += "|VMEvent.BASIC_BLOCK_EXIT";
                    if (p & VMEvent::BASIC_BLOCK_NEW)
                        res += "|VMEvent.BASIC_BLOCK_NEW";
                    if (p & VMEvent::EXEC_TRANSFER_CALL)
                        res += "|VMEvent.EXEC_TRANSFER_CALL";
                    if (p & VMEvent::EXEC_TRANSFER_RETURN)
                        res += "|VMEvent.EXEC_TRANSFER_RETURN";
                    res.erase(0, 1);
                    return res;
                });

    py::enum_<MemoryAccessType>(m, "MemoryAccessType", "Memory access type (read / write / ...)", py::arithmetic())
        .value("MEMORY_READ", MemoryAccessType::MEMORY_READ, "Memory read access")
        .value("MEMORY_WRITE", MemoryAccessType::MEMORY_WRITE, "Memory write access")
        .value("MEMORY_READ_WRITE", MemoryAccessType::MEMORY_READ_WRITE, "Memory read/write access")
        .export_values();

    py::class_<VMState>(m, "VMState")
        .def_readonly("event", &VMState::event,
                "The event(s) which triggered the callback (must be checked using a mask: event & BASIC_BLOCK_ENTRY).")
        .def_readonly("basicBlockStart", &VMState::basicBlockStart,
                "The current basic block start address which can also be the execution transfer destination.")
        .def_readonly("basicBlockEnd", &VMState::basicBlockEnd,
                "The current basic block end address which can also be the execution transfer destination.")
        .def_readonly("sequenceStart", &VMState::sequenceStart,
                "The current sequence start address which can also be the execution transfer destination.")
        .def_readonly("sequenceEnd", &VMState::sequenceEnd,
                "The current sequence end address which can also be the execution transfer destination.");

    py::class_<MemoryAccess>(m, "MemoryAccess")
        .def_readwrite("instAddress", &MemoryAccess::instAddress, "Address of instruction making the access")
        .def_readwrite("accessAddress", &MemoryAccess::accessAddress, "Address of accessed memory")
        .def_readwrite("value", &MemoryAccess::value, "Value read from / written to memory")
        .def_readwrite("size", &MemoryAccess::size, "Size of memory access (in bytes)")
        .def_readwrite("type", &MemoryAccess::type, "Memory access type (READ / WRITE)");
}

}}
