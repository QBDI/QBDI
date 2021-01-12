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

template<class T>
py::object get_InstAnalysis_member(const InstAnalysis& obj, T InstAnalysis::* member, uint32_t v) {
    if (obj.analysisType & v) {
        return py::cast(obj.*member);
    } else {
        return py::none();
    }
}

// specialization for symbol and module if not found
template <>
py::object get_InstAnalysis_member(const InstAnalysis& obj, char* InstAnalysis::* member, uint32_t v) {
    if (obj.analysisType & v && obj.*member != NULL) {
        return py::cast(obj.*member);
    } else {
        return py::none();
    }
}

void init_binding_InstAnalysis(py::module_& m) {

    py::enum_<RegisterAccessType>(m, "RegisterAccessType", py::arithmetic(), "Access type (R/W/RW) of a register operand")
        .value("REGISTER_UNUSED", RegisterAccessType::REGISTER_UNUSED, "Unused register")
        .value("REGISTER_READ", RegisterAccessType::REGISTER_READ, "Register read access")
        .value("REGISTER_WRITE", RegisterAccessType::REGISTER_WRITE, "Register write access")
        .value("REGISTER_READ_WRITE", RegisterAccessType::REGISTER_READ_WRITE, "Register read/write access")
        .export_values();

    py::enum_<OperandType>(m, "OperandType", "Operand type")
        .value("OPERAND_INVALID", OperandType::OPERAND_INVALID, "Invalid operand")
        .value("OPERAND_IMM", OperandType::OPERAND_IMM, "Immediate operand")
        .value("OPERAND_GPR", OperandType::OPERAND_GPR, "Register operand")
        .value("OPERAND_PRED", OperandType::OPERAND_PRED, "Predicate operand")
        .export_values();

    py::enum_<OperandFlag>(m, "OperandFlag", py::arithmetic(), "Operand flag")
        .value("OPERANDFLAG_NONE", OperandFlag::OPERANDFLAG_NONE, "No flag")
        .value("OPERANDFLAG_ADDR", OperandFlag::OPERANDFLAG_ADDR, "The operand is used to compute an address")
        .value("OPERANDFLAG_PCREL", OperandFlag::OPERANDFLAG_PCREL, "The value of the operand is PC relative")
        .value("OPERANDFLAG_UNDEFINED_EFFECT", OperandFlag::OPERANDFLAG_UNDEFINED_EFFECT, "The operand role isn’t fully defined")
        .export_values();

    py::enum_<AnalysisType>(m, "AnalysisType", py::arithmetic(), "Instruction analysis type")
        .value("ANALYSIS_INSTRUCTION", AnalysisType::ANALYSIS_INSTRUCTION, "Instruction analysis (address, mnemonic, ...)")
        .value("ANALYSIS_DISASSEMBLY", AnalysisType::ANALYSIS_DISASSEMBLY, "Instruction disassembly")
        .value("ANALYSIS_OPERANDS", AnalysisType::ANALYSIS_OPERANDS, "Instruction operands analysis")
        .value("ANALYSIS_SYMBOL", AnalysisType::ANALYSIS_SYMBOL, "Instruction symbol")
        .export_values();

    py::class_<OperandAnalysis>(m, "OperandAnalysis")
        .def_readonly("type", &OperandAnalysis::type, "Operand type")
        .def_readonly("flag", &OperandAnalysis::flag, "Operand flag")
        .def_readonly("value", &OperandAnalysis::value, "Operand value (if immediate), or register Id")
        .def_readonly("size", &OperandAnalysis::size, "Operand size (in bytes)")
        .def_readonly("regOff", &OperandAnalysis::regOff, "Sub-register offset in register (in bits)")
        .def_readonly("regCtxIdx", &OperandAnalysis::regCtxIdx, "Register index in VM state")
        .def_readonly("regName", &OperandAnalysis::regName, "Register name")
        .def_readonly("regAccess", &OperandAnalysis::regAccess, "Register access type (r, w, rw)");

    py::class_<InstAnalysis>(m, "InstAnalysis")
        // ANALYSIS_INSTRUCTION
        .def_property_readonly("mnemonic",
                [](const InstAnalysis& obj) {
                    return get_InstAnalysis_member(obj, &InstAnalysis::mnemonic, ANALYSIS_INSTRUCTION);
                }, "LLVM mnemonic (if ANALYSIS_INSTRUCTION)")
        .def_property_readonly("address",
                [](const InstAnalysis& obj) {
                    return get_InstAnalysis_member(obj, &InstAnalysis::address, ANALYSIS_INSTRUCTION);
                }, "Instruction address (if ANALYSIS_INSTRUCTION)")
        .def_property_readonly("instSize",
                [](const InstAnalysis& obj) {
                    return get_InstAnalysis_member(obj, &InstAnalysis::instSize, ANALYSIS_INSTRUCTION);
                }, "Instruction size (in bytes) (if ANALYSIS_INSTRUCTION)")
        .def_property_readonly("affectControlFlow",
                [](const InstAnalysis& obj) {
                    return get_InstAnalysis_member(obj, &InstAnalysis::affectControlFlow, ANALYSIS_INSTRUCTION);
                }, "True if instruction affects control flow (if ANALYSIS_INSTRUCTION)")
        .def_property_readonly("isBranch",
                [](const InstAnalysis& obj) {
                    return get_InstAnalysis_member(obj, &InstAnalysis::isBranch, ANALYSIS_INSTRUCTION);
                }, "True if instruction acts like a 'jump' (if ANALYSIS_INSTRUCTION)")
        .def_property_readonly("isCall",
                [](const InstAnalysis& obj) {
                    return get_InstAnalysis_member(obj, &InstAnalysis::isCall, ANALYSIS_INSTRUCTION);
                }, "True if instruction acts like a 'call' (if ANALYSIS_INSTRUCTION)")
        .def_property_readonly("isReturn",
                [](const InstAnalysis& obj) {
                    return get_InstAnalysis_member(obj, &InstAnalysis::isReturn, ANALYSIS_INSTRUCTION);
                }, "True if instruction acts like a 'return' (if ANALYSIS_INSTRUCTION)")
        .def_property_readonly("isCompare",
                [](const InstAnalysis& obj) {
                    return get_InstAnalysis_member(obj, &InstAnalysis::isCompare, ANALYSIS_INSTRUCTION);
                }, "True if instruction is a comparison (if ANALYSIS_INSTRUCTION)")
        .def_property_readonly("isPredicable",
                [](const InstAnalysis& obj) {
                    return get_InstAnalysis_member(obj, &InstAnalysis::isPredicable, ANALYSIS_INSTRUCTION);
                }, "True if instruction contains a predicate (~is conditional) (if ANALYSIS_INSTRUCTION)")
        .def_property_readonly("mayLoad",
                [](const InstAnalysis& obj) {
                    return get_InstAnalysis_member(obj, &InstAnalysis::mayLoad, ANALYSIS_INSTRUCTION);
                }, "True if instruction 'may' load data from memory (if ANALYSIS_INSTRUCTION)")
        .def_property_readonly("mayStore",
                [](const InstAnalysis& obj) {
                    return get_InstAnalysis_member(obj, &InstAnalysis::mayStore, ANALYSIS_INSTRUCTION);
                }, "True if instruction 'may' store data to memory (if ANALYSIS_INSTRUCTION)")
        // ANALYSIS_DISASSEMBLY
        .def_property_readonly("disassembly",
                [](const InstAnalysis& obj) {
                    return get_InstAnalysis_member(obj, &InstAnalysis::disassembly, ANALYSIS_DISASSEMBLY);
                }, "Instruction disassembly (if ANALYSIS_DISASSEMBLY)")
        // ANALYSIS_OPERANDS
        .def_property_readonly("numOperands",
                [](const InstAnalysis& obj) {
                    return get_InstAnalysis_member(obj, &InstAnalysis::numOperands, ANALYSIS_OPERANDS);
                }, "Number of operands used by the instruction (if ANALYSIS_OPERANDS)")
        .def_property_readonly("operands",
                [](const InstAnalysis& obj) {
                    if (obj.analysisType & ANALYSIS_OPERANDS) {
                        std::vector<OperandAnalysis*> operandslist;
                        for (int i = 0; i < obj.numOperands; i++) {
                            operandslist.push_back(&(obj.operands[i]));
                        }
                        return static_cast<py::object>(py::tuple(py::cast(operandslist)));
                    } else {
                        return static_cast<py::object>(py::none());
                    }
                }, "Structure containing analysis results of an operand provided by the VM (if ANALYSIS_OPERANDS)")
        // ANALYSIS_SYMBOL
        .def_property_readonly("symbol",
                [](const InstAnalysis& obj) {
                    return get_InstAnalysis_member(obj, &InstAnalysis::symbol, ANALYSIS_SYMBOL);
                }, "Instruction symbol (if ANALYSIS_SYMBOL and found)")
        .def_property_readonly("symbolOffset",
                [](const InstAnalysis& obj) {
                    return get_InstAnalysis_member(obj, &InstAnalysis::symbolOffset, ANALYSIS_SYMBOL);
                }, "Instruction symbol offset (if ANALYSIS_SYMBOL)")
        .def_property_readonly("module",
                [](const InstAnalysis& obj) {
                    return get_InstAnalysis_member(obj, &InstAnalysis::module, ANALYSIS_SYMBOL);
                }, "Instruction module name (if ANALYSIS_SYMBOL and found)");


}

}}
