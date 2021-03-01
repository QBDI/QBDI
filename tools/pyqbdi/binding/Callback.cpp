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
#include "Enum.hpp"
#include "callback_python.h"

namespace QBDI {
namespace pyQBDI {

void init_binding_Callback(py::module_& m) {

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

    enum_int_flag_<VMEvent>(m, "VMEvent", py::arithmetic())
        .value("SEQUENCE_ENTRY", VMEvent::SEQUENCE_ENTRY, "Triggered when the execution enters a sequence.")
        .value("SEQUENCE_EXIT", VMEvent::SEQUENCE_EXIT, "Triggered when the execution exits from the current sequence.")
        .value("BASIC_BLOCK_ENTRY", VMEvent::BASIC_BLOCK_ENTRY, "Triggered when the execution enters a basic block.")
        .value("BASIC_BLOCK_EXIT", VMEvent::BASIC_BLOCK_EXIT, "Triggered when the execution exits from the current basic block.")
        .value("BASIC_BLOCK_NEW", VMEvent::BASIC_BLOCK_NEW, "Triggered when the execution enters a new (~unknown) basic block.")
        .value("EXEC_TRANSFER_CALL", VMEvent::EXEC_TRANSFER_CALL, "Triggered when the ExecBroker executes an execution transfer.")
        .value("EXEC_TRANSFER_RETURN", VMEvent::EXEC_TRANSFER_RETURN, "Triggered when the ExecBroker returns from an execution transfer.")
        .export_values()
        .def_invert()
        .def_repr_str();

    enum_int_flag_<MemoryAccessType>(m, "MemoryAccessType", "Memory access type (read / write / ...)", py::arithmetic())
        .value("MEMORY_READ", MemoryAccessType::MEMORY_READ, "Memory read access")
        .value("MEMORY_WRITE", MemoryAccessType::MEMORY_WRITE, "Memory write access")
        .value("MEMORY_READ_WRITE", MemoryAccessType::MEMORY_READ_WRITE, "Memory read/write access")
        .export_values()
        .def_invert();

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
                "The current sequence end address which can also be the execution transfer destination.")
        .def_readonly("returnAddressPtr", &VMState::returnAddressPtr,
                "The address of the return address detected during EXEC_TRANSFER_CALL.")
        .def_readonly("returnAddressValue", &VMState::returnAddressValue,
                "The return address detected during EXEC_TRANSFER_CALL.");

    enum_int_flag_<MemoryAccessFlags>(m, "MemoryAccessFlags", "Memory access flags", py::arithmetic())
        .value("MEMORY_NO_FLAGS", MemoryAccessFlags::MEMORY_NO_FLAGS, "Empty flags")
        .value("MEMORY_UNKNOWN_SIZE", MemoryAccessFlags::MEMORY_UNKNOWN_SIZE, "The size of the access isn't known.")
        .value("MEMORY_MINIMUM_SIZE", MemoryAccessFlags::MEMORY_MINIMUM_SIZE, "The given size is a minimum size.")
        .value("MEMORY_UNKNOWN_VALUE", MemoryAccessFlags::MEMORY_UNKNOWN_VALUE, "The value of the access is unknown or hasn't been retrived.")
        .export_values()
        .def_invert()
        .def_repr_str();

    py::class_<MemoryAccess>(m, "MemoryAccess")
        .def_readwrite("instAddress", &MemoryAccess::instAddress, "Address of instruction making the access")
        .def_readwrite("accessAddress", &MemoryAccess::accessAddress, "Address of accessed memory")
        .def_readwrite("value", &MemoryAccess::value, "Value read from / written to memory")
        .def_readwrite("size", &MemoryAccess::size, "Size of memory access (in bytes)")
        .def_readwrite("type", &MemoryAccess::type, "Memory access type (READ / WRITE)")
        .def_readwrite("flags", &MemoryAccess::flags, "Memory access flags");

    py::class_<InstrRuleDataCBKPython>(m, "InstrRuleDataCBK")
        .def(py::init<PyInstCallback&, py::object&, InstPosition>(), "cbk"_a, "data"_a, "position"_a)
        .def_readwrite("cbk", &InstrRuleDataCBKPython::cbk, "Address of the function to call when the instruction is executed")
        .def_readwrite("data", &InstrRuleDataCBKPython::data, "User defined data which will be forward to cbk.")
        .def_readwrite("position", &InstrRuleDataCBKPython::position, "Relative position of the event callback (PREINST / POSTINST).");
}

}}
