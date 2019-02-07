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

    py::enum_<QBDI::VMAction>(m, "VMAction", "The callback results.")
        .value("CONTINUE", QBDI::VMAction::CONTINUE, "The execution of the basic block continues.")
        .value("BREAK_TO_VM", QBDI::VMAction::BREAK_TO_VM,
                "The execution breaks and returns to the VM causing a complete reevaluation of "
                "the execution state. A BREAK_TO_VM is needed to ensure that modifications of "
                "the Program Counter or the program code are taken into account.")
        .value("STOP", QBDI::VMAction::STOP,
                "Stops the execution of the program. This causes the run function to return early.")
        .export_values();

    py::enum_<QBDI::InstPosition>(m, "InstPosition", "Position relative to an instruction.")
        .value("PREINST", QBDI::InstPosition::PREINST, "Positioned before the instruction.")
        .value("POSTINST", QBDI::InstPosition::POSTINST, "Positioned after the instruction.")
        .export_values();

    py::enum_<QBDI::VMEvent>(m, "VMEvent", py::arithmetic())
        .value("SEQUENCE_ENTRY", QBDI::VMEvent::SEQUENCE_ENTRY, "Triggered when the execution enters a sequence.")
        .value("SEQUENCE_EXIT", QBDI::VMEvent::SEQUENCE_EXIT, "Triggered when the execution exits from the current sequence.")
        .value("BASIC_BLOCK_ENTRY", QBDI::VMEvent::BASIC_BLOCK_ENTRY, "Triggered when the execution enters a basic block.")
        .value("BASIC_BLOCK_EXIT", QBDI::VMEvent::BASIC_BLOCK_EXIT, "Triggered when the execution exits from the current basic block.")
        .value("BASIC_BLOCK_NEW", QBDI::VMEvent::BASIC_BLOCK_NEW, "Triggered when the execution enters a new (~unknown) basic block.")
        .value("EXEC_TRANSFER_CALL", QBDI::VMEvent::EXEC_TRANSFER_CALL, "Triggered when the ExecBroker executes an execution transfer.")
        .value("EXEC_TRANSFER_RETURN", QBDI::VMEvent::EXEC_TRANSFER_RETURN, "Triggered when the ExecBroker returns from an execution transfer.")
        .export_values()
        .def("__str__",
                [](const QBDI::VMEvent p) {
                    std::string res;
                    if (p & QBDI::VMEvent::SEQUENCE_ENTRY)
                        res += "|VMEvent.SEQUENCE_ENTRY";
                    if (p & QBDI::VMEvent::SEQUENCE_EXIT)
                        res += "|VMEvent.SEQUENCE_EXIT";
                    if (p & QBDI::VMEvent::BASIC_BLOCK_ENTRY)
                        res += "|VMEvent.BASIC_BLOCK_ENTRY";
                    if (p & QBDI::VMEvent::BASIC_BLOCK_EXIT)
                        res += "|VMEvent.BASIC_BLOCK_EXIT";
                    if (p & QBDI::VMEvent::BASIC_BLOCK_NEW)
                        res += "|VMEvent.BASIC_BLOCK_NEW";
                    if (p & QBDI::VMEvent::EXEC_TRANSFER_CALL)
                        res += "|VMEvent.EXEC_TRANSFER_CALL";
                    if (p & QBDI::VMEvent::EXEC_TRANSFER_RETURN)
                        res += "|VMEvent.EXEC_TRANSFER_RETURN";
                    res.erase(0, 1);
                    return res;
                });

    py::enum_<QBDI::MemoryAccessType>(m, "MemoryAccessType", "Memory access type (read / write / ...)", py::arithmetic())
        .value("MEMORY_READ", QBDI::MemoryAccessType::MEMORY_READ, "Memory read access")
        .value("MEMORY_WRITE", QBDI::MemoryAccessType::MEMORY_WRITE, "Memory write access")
        .value("MEMORY_READ_WRITE", QBDI::MemoryAccessType::MEMORY_READ_WRITE, "Memory read/write access")
        .export_values();

    py::class_<QBDI::VMState>(m, "VMState")
        .def(py::init<>())
        .def_readonly("event", &QBDI::VMState::event,
                "The event(s) which triggered the callback (must be checked using a mask: event & BASIC_BLOCK_ENTRY).")
        .def_readonly("basicBlockStart", &QBDI::VMState::basicBlockStart,
                "The current basic block start address which can also be the execution transfer destination.")
        .def_readonly("basicBlockEnd", &QBDI::VMState::basicBlockEnd,
                "The current basic block end address which can also be the execution transfer destination.")
        .def_readonly("sequenceStart", &QBDI::VMState::sequenceStart,
                "The current sequence start address which can also be the execution transfer destination.")
        .def_readonly("sequenceEnd", &QBDI::VMState::sequenceEnd,
                "The current sequence end address which can also be the execution transfer destination.");

    py::class_<QBDI::MemoryAccess>(m, "MemoryAccess")
        .def(py::init<>())
        .def_readwrite("instAddress", &QBDI::MemoryAccess::instAddress, "Address of instruction making the access")
        .def_readwrite("accessAddress", &QBDI::MemoryAccess::accessAddress, "Address of accessed memory")
        .def_readwrite("value", &QBDI::MemoryAccess::value, "Value read from / written to memory")
        .def_readwrite("size", &QBDI::MemoryAccess::size, "Size of memory access (in bytes)")
        .def_readwrite("type", &QBDI::MemoryAccess::type, "Memory access type (READ / WRITE)");
}

}}
