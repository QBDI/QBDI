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

void hexify_register(std::ostringstream& out, const char* s, int size) {
    for (int i = size-1; i >= 0; i--) {
        out << std::setw(2) << (static_cast<unsigned>(s[i]) & 0xff);
    }
}

void init_binding_State(py::module& m) {

    py::class_<QBDI::FPControl>(m, "FPControl")
        .def(py::init<>())
        .def_property("invalid",
                [](const QBDI::FPControl &t) {return t.invalid;},
                [](QBDI::FPControl &t, int v){t.invalid = v & 1;})
        .def_property("denorm",
                [](const QBDI::FPControl &t) {return t.denorm;},
                [](QBDI::FPControl &t, int v){t.denorm = v & 1;})
        .def_property("zdiv",
                [](const QBDI::FPControl &t) {return t.zdiv;},
                [](QBDI::FPControl &t, int v){t.zdiv = v & 1;})
        .def_property("ovrfl",
                [](const QBDI::FPControl &t) {return t.ovrfl;},
                [](QBDI::FPControl &t, int v){t.ovrfl = v & 1;})
        .def_property("undfl",
                [](const QBDI::FPControl &t) {return t.undfl;},
                [](QBDI::FPControl &t, int v){t.undfl = v & 1;})
        .def_property("precis",
                [](const QBDI::FPControl &t) {return t.precis;},
                [](QBDI::FPControl &t, int v){t.precis = v & 1;})
        .def_property("pc",
                [](const QBDI::FPControl &t) {return t.pc;},
                [](QBDI::FPControl &t, int v){t.pc = v & 3;})
        .def_property("rc",
                [](const QBDI::FPControl &t) {return t.rc;},
                [](QBDI::FPControl &t, int v){t.rc = v & 3;});

    py::class_<QBDI::FPStatus>(m, "FPStatus")
        .def(py::init<>())
        .def_property("invalid",
                [](const QBDI::FPStatus &t) {return t.invalid;},
                [](QBDI::FPStatus &t, int v){t.invalid = v & 1;})
        .def_property("denorm",
                [](const QBDI::FPStatus &t) {return t.denorm;},
                [](QBDI::FPStatus &t, int v){t.denorm = v & 1;})
        .def_property("zdiv",
                [](const QBDI::FPStatus &t) {return t.zdiv;},
                [](QBDI::FPStatus &t, int v){t.zdiv = v & 1;})
        .def_property("ovrfl",
                [](const QBDI::FPStatus &t) {return t.ovrfl;},
                [](QBDI::FPStatus &t, int v){t.ovrfl = v & 1;})
        .def_property("undfl",
                [](const QBDI::FPStatus &t) {return t.undfl;},
                [](QBDI::FPStatus &t, int v){t.undfl = v & 1;})
        .def_property("precis",
                [](const QBDI::FPStatus &t) {return t.precis;},
                [](QBDI::FPStatus &t, int v){t.precis = v & 1;})
        .def_property("stkflt",
                [](const QBDI::FPStatus &t) {return t.stkflt;},
                [](QBDI::FPStatus &t, int v){t.stkflt = v & 1;})
        .def_property("errsumm",
                [](const QBDI::FPStatus &t) {return t.errsumm;},
                [](QBDI::FPStatus &t, int v){t.errsumm = v & 1;})
        .def_property("c0",
                [](const QBDI::FPStatus &t) {return t.c0;},
                [](QBDI::FPStatus &t, int v){t.c0 = v & 1;})
        .def_property("c1",
                [](const QBDI::FPStatus &t) {return t.c1;},
                [](QBDI::FPStatus &t, int v){t.c1 = v & 1;})
        .def_property("c2",
                [](const QBDI::FPStatus &t) {return t.c2;},
                [](QBDI::FPStatus &t, int v){t.c2 = v & 1;})
        .def_property("tos",
                [](const QBDI::FPStatus &t) {return t.tos;},
                [](QBDI::FPStatus &t, int v){t.tos = v & 7;})
        .def_property("c3",
                [](const QBDI::FPStatus &t) {return t.c3;},
                [](QBDI::FPStatus &t, int v){t.c3 = v & 1;})
        .def_property("busy",
                [](const QBDI::FPStatus &t) {return t.busy;},
                [](QBDI::FPStatus &t, int v){t.busy = v & 1;});

    py::class_<QBDI::MMSTReg>(m, "MMSTReg")
        .def(py::init<>())
        .def_property("st",
                [](const QBDI::MMSTReg &t) {return py::bytes(t.reg, sizeof(t.reg));},
                [](QBDI::MMSTReg &t, py::bytes v) {std::string(v).copy(t.reg, sizeof(t.reg),0);});

    py::class_<QBDI::FPRState>(m, "FPRState")
        .def(py::init<>())
        .def_readwrite("fcw", &QBDI::FPRState::fcw, "x87 FPU control word")
        .def_readwrite("rfcw", &QBDI::FPRState::rfcw, "x87 FPU control word")
        .def_readwrite("fsw", &QBDI::FPRState::fsw, "x87 FPU status word")
        .def_readwrite("rfsw", &QBDI::FPRState::rfsw, "x87 FPU status word")
        .def_readwrite("ftw", &QBDI::FPRState::ftw, "x87 FPU tag word")
        .def_readwrite("fop", &QBDI::FPRState::fop, "x87 FPU Opcode")
        .def_readwrite("ip", &QBDI::FPRState::ip, "x87 FPU Instruction Pointer offset")
        .def_readwrite("cs", &QBDI::FPRState::cs, "x87 FPU Instruction Pointer Selector")
        .def_readwrite("dp", &QBDI::FPRState::dp, "x87 FPU Instruction Operand(Data) Pointer offset")
        .def_readwrite("ds", &QBDI::FPRState::ds, "x87 FPU Instruction Operand(Data) Pointer Selector")
        .def_readwrite("mxcsr", &QBDI::FPRState::mxcsr, "MXCSR Register state")
        .def_readwrite("mxcsrmask", &QBDI::FPRState::mxcsrmask, "MXCSR mask")
        .def_readwrite("stmm0", &QBDI::FPRState::stmm0, "ST0/MM0")
        .def_readwrite("stmm1", &QBDI::FPRState::stmm1, "ST1/MM1")
        .def_readwrite("stmm2", &QBDI::FPRState::stmm2, "ST2/MM2")
        .def_readwrite("stmm3", &QBDI::FPRState::stmm3, "ST3/MM3")
        .def_readwrite("stmm4", &QBDI::FPRState::stmm4, "ST4/MM4")
        .def_readwrite("stmm5", &QBDI::FPRState::stmm5, "ST5/MM5")
        .def_readwrite("stmm6", &QBDI::FPRState::stmm6, "ST6/MM6")
        .def_readwrite("stmm7", &QBDI::FPRState::stmm7, "ST7/MM7")
        .def_property("xmm0",
                [](const QBDI::FPRState &t) {return py::bytes(t.xmm0, sizeof(t.xmm0));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.xmm0, sizeof(t.xmm0),0);},
                "XMM 0")
        .def_property("xmm1",
                [](const QBDI::FPRState &t) {return py::bytes(t.xmm1, sizeof(t.xmm1));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.xmm1, sizeof(t.xmm1),0);},
                "XMM 1")
        .def_property("xmm2",
                [](const QBDI::FPRState &t) {return py::bytes(t.xmm2, sizeof(t.xmm2));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.xmm2, sizeof(t.xmm2),0);},
                "XMM 2")
        .def_property("xmm3",
                [](const QBDI::FPRState &t) {return py::bytes(t.xmm3, sizeof(t.xmm3));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.xmm3, sizeof(t.xmm3),0);},
                "XMM 3")
        .def_property("xmm4",
                [](const QBDI::FPRState &t) {return py::bytes(t.xmm4, sizeof(t.xmm4));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.xmm4, sizeof(t.xmm4),0);},
                "XMM 4")
        .def_property("xmm5",
                [](const QBDI::FPRState &t) {return py::bytes(t.xmm5, sizeof(t.xmm5));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.xmm5, sizeof(t.xmm5),0);},
                "XMM 5")
        .def_property("xmm6",
                [](const QBDI::FPRState &t) {return py::bytes(t.xmm6, sizeof(t.xmm6));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.xmm6, sizeof(t.xmm6),0);},
                "XMM 6")
        .def_property("xmm7",
                [](const QBDI::FPRState &t) {return py::bytes(t.xmm7, sizeof(t.xmm7));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.xmm7, sizeof(t.xmm7),0);},
                "XMM 7")
        .def_property("xmm8",
                [](const QBDI::FPRState &t) {return py::bytes(t.xmm8, sizeof(t.xmm8));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.xmm8, sizeof(t.xmm8),0);},
                "XMM 8")
        .def_property("xmm9",
                [](const QBDI::FPRState &t) {return py::bytes(t.xmm9, sizeof(t.xmm9));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.xmm9, sizeof(t.xmm9),0);},
                "XMM 9")
        .def_property("xmm10",
                [](const QBDI::FPRState &t) {return py::bytes(t.xmm10, sizeof(t.xmm10));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.xmm10, sizeof(t.xmm10),0);},
                "XMM 10")
        .def_property("xmm11",
                [](const QBDI::FPRState &t) {return py::bytes(t.xmm11, sizeof(t.xmm11));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.xmm11, sizeof(t.xmm11),0);},
                "XMM 11")
        .def_property("xmm12",
                [](const QBDI::FPRState &t) {return py::bytes(t.xmm12, sizeof(t.xmm12));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.xmm12, sizeof(t.xmm12),0);},
                "XMM 12")
        .def_property("xmm13",
                [](const QBDI::FPRState &t) {return py::bytes(t.xmm13, sizeof(t.xmm13));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.xmm13, sizeof(t.xmm13),0);},
                "XMM 13")
        .def_property("xmm14",
                [](const QBDI::FPRState &t) {return py::bytes(t.xmm14, sizeof(t.xmm14));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.xmm14, sizeof(t.xmm14),0);},
                "XMM 14")
        .def_property("xmm15",
                [](const QBDI::FPRState &t) {return py::bytes(t.xmm15, sizeof(t.xmm15));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.xmm15, sizeof(t.xmm15),0);},
                "XMM 15")
        .def_property("ymm0",
                [](const QBDI::FPRState &t) {return py::bytes(t.ymm0, sizeof(t.ymm0));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.ymm0, sizeof(t.ymm0),0);},
                "YMM0[255:128]")
        .def_property("ymm1",
                [](const QBDI::FPRState &t) {return py::bytes(t.ymm1, sizeof(t.ymm1));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.ymm1, sizeof(t.ymm1),0);},
                "YMM1[255:128]")
        .def_property("ymm2",
                [](const QBDI::FPRState &t) {return py::bytes(t.ymm2, sizeof(t.ymm2));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.ymm2, sizeof(t.ymm2),0);},
                "YMM2[255:128]")
        .def_property("ymm3",
                [](const QBDI::FPRState &t) {return py::bytes(t.ymm3, sizeof(t.ymm3));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.ymm3, sizeof(t.ymm3),0);},
                "YMM3[255:128]")
        .def_property("ymm4",
                [](const QBDI::FPRState &t) {return py::bytes(t.ymm4, sizeof(t.ymm4));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.ymm4, sizeof(t.ymm4),0);},
                "YMM4[255:128]")
        .def_property("ymm5",
                [](const QBDI::FPRState &t) {return py::bytes(t.ymm5, sizeof(t.ymm5));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.ymm5, sizeof(t.ymm5),0);},
                "YMM5[255:128]")
        .def_property("ymm6",
                [](const QBDI::FPRState &t) {return py::bytes(t.ymm6, sizeof(t.ymm6));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.ymm6, sizeof(t.ymm6),0);},
                "YMM6[255:128]")
        .def_property("ymm7",
                [](const QBDI::FPRState &t) {return py::bytes(t.ymm7, sizeof(t.ymm7));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.ymm7, sizeof(t.ymm7),0);},
                "YMM7[255:128]")
        .def_property("ymm8",
                [](const QBDI::FPRState &t) {return py::bytes(t.ymm8, sizeof(t.ymm8));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.ymm8, sizeof(t.ymm8),0);},
                "YMM8[255:128]")
        .def_property("ymm9",
                [](const QBDI::FPRState &t) {return py::bytes(t.ymm9, sizeof(t.ymm9));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.ymm9, sizeof(t.ymm9),0);},
                "YMM9[255:128]")
        .def_property("ymm10",
                [](const QBDI::FPRState &t) {return py::bytes(t.ymm10, sizeof(t.ymm10));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.ymm10, sizeof(t.ymm10),0);},
                "YMM10[255:128]")
        .def_property("ymm11",
                [](const QBDI::FPRState &t) {return py::bytes(t.ymm11, sizeof(t.ymm11));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.ymm11, sizeof(t.ymm11),0);},
                "YMM11[255:128]")
        .def_property("ymm12",
                [](const QBDI::FPRState &t) {return py::bytes(t.ymm12, sizeof(t.ymm12));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.ymm12, sizeof(t.ymm12),0);},
                "YMM12[255:128]")
        .def_property("ymm13",
                [](const QBDI::FPRState &t) {return py::bytes(t.ymm13, sizeof(t.ymm13));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.ymm13, sizeof(t.ymm13),0);},
                "YMM13[255:128]")
        .def_property("ymm14",
                [](const QBDI::FPRState &t) {return py::bytes(t.ymm14, sizeof(t.ymm14));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.ymm14, sizeof(t.ymm14),0);},
                "YMM14[255:128]")
        .def_property("ymm15",
                [](const QBDI::FPRState &t) {return py::bytes(t.ymm15, sizeof(t.ymm15));},
                [](QBDI::FPRState &t, py::bytes v) {std::string(v).copy(t.ymm15, sizeof(t.ymm15),0);},
                "YMM15[255:128]")
        .def("__str__", [](const QBDI::FPRState& obj) {
                               std::ostringstream oss;
                               oss << std::hex << std::setfill('0')
                                   << "=== FPRState begin ===" << std::endl
                                   << "rfcw  : 0x" << std::setw(sizeof(uint16_t)*2) << obj.rfcw << std::endl
                                   << "rfsw  : 0x" << std::setw(sizeof(uint16_t)*2) << obj.rfsw << std::endl
                                   << "ftw   : 0x" << std::setw(sizeof(uint8_t)*2)  << (static_cast<unsigned>(obj.ftw) & 0xff) << std::endl
                                   << "fop   : 0x" << std::setw(sizeof(uint16_t)*2) << obj.fop << std::endl
                                   << "ip    : 0x" << std::setw(sizeof(uint32_t)*2) << obj.ip << std::endl
                                   << "cs    : 0x" << std::setw(sizeof(uint16_t)*2) << obj.cs << std::endl
                                   << "dp    : 0x" << std::setw(sizeof(uint32_t)*2) << obj.dp << std::endl
                                   << "ds    : 0x" << std::setw(sizeof(uint16_t)*2) << obj.ds << std::endl
                                   << "mxcsr : 0x" << std::setw(sizeof(uint32_t)*2) << obj.mxcsr << std::endl
                                   << "mxcsrmask : 0x" << std::setw(sizeof(uint32_t)*2) << obj.mxcsrmask << std::endl
                                   << "stmm0 : 0x";
                                hexify_register(oss, obj.stmm0.reg, sizeof(obj.stmm0.reg));
                                oss << std::endl << "stmm1 : 0x";
                                hexify_register(oss, obj.stmm1.reg, sizeof(obj.stmm1.reg));
                                oss << std::endl << "stmm2 : 0x";
                                hexify_register(oss, obj.stmm2.reg, sizeof(obj.stmm2.reg));
                                oss << std::endl << "stmm3 : 0x";
                                hexify_register(oss, obj.stmm3.reg, sizeof(obj.stmm3.reg));
                                oss << std::endl << "stmm4 : 0x";
                                hexify_register(oss, obj.stmm4.reg, sizeof(obj.stmm4.reg));
                                oss << std::endl << "stmm5 : 0x";
                                hexify_register(oss, obj.stmm5.reg, sizeof(obj.stmm5.reg));
                                oss << std::endl << "stmm6 : 0x";
                                hexify_register(oss, obj.stmm6.reg, sizeof(obj.stmm6.reg));
                                oss << std::endl << "stmm7 : 0x";
                                hexify_register(oss, obj.stmm7.reg, sizeof(obj.stmm7.reg));

                                oss << std::endl << "xmm0  : 0x";
                                hexify_register(oss, obj.xmm0, sizeof(obj.xmm0));
                                oss << std::endl << "xmm1  : 0x";
                                hexify_register(oss, obj.xmm1, sizeof(obj.xmm1));
                                oss << std::endl << "xmm2  : 0x";
                                hexify_register(oss, obj.xmm2, sizeof(obj.xmm2));
                                oss << std::endl << "xmm3  : 0x";
                                hexify_register(oss, obj.xmm3, sizeof(obj.xmm3));
                                oss << std::endl << "xmm4  : 0x";
                                hexify_register(oss, obj.xmm4, sizeof(obj.xmm4));
                                oss << std::endl << "xmm5  : 0x";
                                hexify_register(oss, obj.xmm5, sizeof(obj.xmm5));
                                oss << std::endl << "xmm6  : 0x";
                                hexify_register(oss, obj.xmm6, sizeof(obj.xmm6));
                                oss << std::endl << "xmm7  : 0x";
                                hexify_register(oss, obj.xmm7, sizeof(obj.xmm7));
                                oss << std::endl << "xmm8  : 0x";
                                hexify_register(oss, obj.xmm8, sizeof(obj.xmm8));
                                oss << std::endl << "xmm9  : 0x";
                                hexify_register(oss, obj.xmm9, sizeof(obj.xmm9));
                                oss << std::endl << "xmm10 : 0x";
                                hexify_register(oss, obj.xmm10, sizeof(obj.xmm10));
                                oss << std::endl << "xmm11 : 0x";
                                hexify_register(oss, obj.xmm11, sizeof(obj.xmm11));
                                oss << std::endl << "xmm12 : 0x";
                                hexify_register(oss, obj.xmm12, sizeof(obj.xmm12));
                                oss << std::endl << "xmm13 : 0x";
                                hexify_register(oss, obj.xmm13, sizeof(obj.xmm13));
                                oss << std::endl << "xmm14 : 0x";
                                hexify_register(oss, obj.xmm14, sizeof(obj.xmm14));
                                oss << std::endl << "xmm15 : 0x";
                                hexify_register(oss, obj.xmm15, sizeof(obj.xmm15));

                                oss << std::endl << "ymm0  : 0x";
                                hexify_register(oss, obj.ymm0, sizeof(obj.ymm0));
                                hexify_register(oss, obj.xmm0, sizeof(obj.xmm0));
                                oss << std::endl << "ymm1  : 0x";
                                hexify_register(oss, obj.ymm1, sizeof(obj.ymm1));
                                hexify_register(oss, obj.xmm1, sizeof(obj.xmm1));
                                oss << std::endl << "ymm2  : 0x";
                                hexify_register(oss, obj.ymm2, sizeof(obj.ymm2));
                                hexify_register(oss, obj.xmm2, sizeof(obj.xmm2));
                                oss << std::endl << "ymm3  : 0x";
                                hexify_register(oss, obj.ymm3, sizeof(obj.ymm3));
                                hexify_register(oss, obj.xmm3, sizeof(obj.xmm3));
                                oss << std::endl << "ymm4  : 0x";
                                hexify_register(oss, obj.ymm4, sizeof(obj.ymm4));
                                hexify_register(oss, obj.xmm4, sizeof(obj.xmm4));
                                oss << std::endl << "ymm5  : 0x";
                                hexify_register(oss, obj.ymm5, sizeof(obj.ymm5));
                                hexify_register(oss, obj.xmm5, sizeof(obj.xmm5));
                                oss << std::endl << "ymm6  : 0x";
                                hexify_register(oss, obj.ymm6, sizeof(obj.ymm6));
                                hexify_register(oss, obj.xmm6, sizeof(obj.xmm6));
                                oss << std::endl << "ymm7  : 0x";
                                hexify_register(oss, obj.ymm7, sizeof(obj.ymm7));
                                hexify_register(oss, obj.xmm7, sizeof(obj.xmm7));
                                oss << std::endl << "ymm8  : 0x";
                                hexify_register(oss, obj.ymm8, sizeof(obj.ymm8));
                                hexify_register(oss, obj.xmm8, sizeof(obj.xmm8));
                                oss << std::endl << "ymm9  : 0x";
                                hexify_register(oss, obj.ymm9, sizeof(obj.ymm9));
                                hexify_register(oss, obj.xmm9, sizeof(obj.xmm9));
                                oss << std::endl << "ymm10 : 0x";
                                hexify_register(oss, obj.ymm10, sizeof(obj.ymm10));
                                hexify_register(oss, obj.xmm10, sizeof(obj.xmm10));
                                oss << std::endl << "ymm11 : 0x";
                                hexify_register(oss, obj.ymm11, sizeof(obj.ymm11));
                                hexify_register(oss, obj.xmm11, sizeof(obj.xmm11));
                                oss << std::endl << "ymm12 : 0x";
                                hexify_register(oss, obj.ymm12, sizeof(obj.ymm12));
                                hexify_register(oss, obj.xmm12, sizeof(obj.xmm12));
                                oss << std::endl << "ymm13 : 0x";
                                hexify_register(oss, obj.ymm13, sizeof(obj.ymm13));
                                hexify_register(oss, obj.xmm13, sizeof(obj.xmm13));
                                oss << std::endl << "ymm14 : 0x";
                                hexify_register(oss, obj.ymm14, sizeof(obj.ymm14));
                                hexify_register(oss, obj.xmm14, sizeof(obj.xmm14));
                                oss << std::endl << "ymm15 : 0x";
                                hexify_register(oss, obj.ymm15, sizeof(obj.ymm15));
                                hexify_register(oss, obj.xmm15, sizeof(obj.xmm15));

                                oss << std::endl << "=== FPRState end ===" << std::endl;
                               return oss.str();
                           });

    m.attr("REG_RETURN") = REG_RETURN;
    m.attr("AVAILABLE_GPR") = AVAILABLE_GPR;
    m.attr("REG_BP") = REG_BP;
    m.attr("REG_SP") = REG_SP;
    m.attr("REG_PC") = REG_PC;
    m.attr("NUM_GPR") = NUM_GPR;
    m.attr("REG_LR") = py::none();

    py::class_<QBDI::GPRState>(m, "GPRState")
        .def(py::init<>())
        .def_readwrite("rax", &QBDI::GPRState::rax)
        .def_readwrite("rbx", &QBDI::GPRState::rbx)
        .def_readwrite("rcx", &QBDI::GPRState::rcx)
        .def_readwrite("rdx", &QBDI::GPRState::rdx)
        .def_readwrite("rsi", &QBDI::GPRState::rsi)
        .def_readwrite("rdi", &QBDI::GPRState::rdi)
        .def_readwrite("r8", &QBDI::GPRState::r8)
        .def_readwrite("r9", &QBDI::GPRState::r9)
        .def_readwrite("r10", &QBDI::GPRState::r10)
        .def_readwrite("r11", &QBDI::GPRState::r11)
        .def_readwrite("r12", &QBDI::GPRState::r12)
        .def_readwrite("r13", &QBDI::GPRState::r13)
        .def_readwrite("r14", &QBDI::GPRState::r14)
        .def_readwrite("r15", &QBDI::GPRState::r15)
        .def_readwrite("rbp", &QBDI::GPRState::rbp)
        .def_readwrite("rsp", &QBDI::GPRState::rsp)
        .def_readwrite("rip", &QBDI::GPRState::rip)
        .def_readwrite("eflags", &QBDI::GPRState::eflags)
        // cross architecture access
        .def_readwrite("REG_RETURN", &QBDI::GPRState::rax, "shadow of rax")
        .def_readwrite("AVAILABLE_GPR", &QBDI::GPRState::rbp, "shadow of rbp")
        .def_readwrite("REG_BP", &QBDI::GPRState::rbp, "shadow of rbp")
        .def_readwrite("REG_SP", &QBDI::GPRState::rsp, "shadow of rsp")
        .def_readwrite("REG_PC", &QBDI::GPRState::rip, "shadow of rip")
        .def_readwrite("NUM_GPR", &QBDI::GPRState::eflags, "shadow of eflags")
        .def_property_readonly("REG_LR",
                [](const QBDI::GPRState& obj) {return py::none();}, "not available on X86_64")
        .def("__str__", [](const QBDI::GPRState& obj) {
                               std::ostringstream oss;
                               int r = sizeof(rword) * 2;
                               oss << std::hex << std::setfill('0')
                                   << "=== GPRState begin ===" << std::endl
                                   << "rax    : 0x" << std::setw(r) << obj.rax << std::endl
                                   << "rbx    : 0x" << std::setw(r) << obj.rbx << std::endl
                                   << "rcx    : 0x" << std::setw(r) << obj.rcx << std::endl
                                   << "rdx    : 0x" << std::setw(r) << obj.rdx << std::endl
                                   << "rsi    : 0x" << std::setw(r) << obj.rsi << std::endl
                                   << "rdi    : 0x" << std::setw(r) << obj.rdi << std::endl
                                   << "r8     : 0x" << std::setw(r) << obj.r8 << std::endl
                                   << "r9     : 0x" << std::setw(r) << obj.r9 << std::endl
                                   << "r10    : 0x" << std::setw(r) << obj.r10 << std::endl
                                   << "r11    : 0x" << std::setw(r) << obj.r11 << std::endl
                                   << "r12    : 0x" << std::setw(r) << obj.r12 << std::endl
                                   << "r13    : 0x" << std::setw(r) << obj.r13 << std::endl
                                   << "r14    : 0x" << std::setw(r) << obj.r14 << std::endl
                                   << "r15    : 0x" << std::setw(r) << obj.r15 << std::endl
                                   << "rbp    : 0x" << std::setw(r) << obj.rbp << std::endl
                                   << "rsp    : 0x" << std::setw(r) << obj.rsp << std::endl
                                   << "rip    : 0x" << std::setw(r) << obj.rip << std::endl
                                   << "eflags : 0x" << std::setw(r) << obj.eflags << std::endl
                                   << "=== GPRState end ===" << std::endl;
                               return oss.str();
                           })
        .def("__getitem__", [](const QBDI::GPRState& obj, unsigned int index) {
                    if (index >= (sizeof(QBDI::GPRState) / sizeof(QBDI::rword))) {
                        throw pybind11::index_error("Out of range of QBDI::GPRState");
                    }
                    return QBDI_GPR_GET(&obj, index);
                })
        .def("__setitem__", [](QBDI::GPRState& obj, unsigned int index, QBDI::rword value) {
                    if (index >= (sizeof(QBDI::GPRState) / sizeof(QBDI::rword))) {
                        throw pybind11::index_error("Out of range of QBDI::GPRState");
                    }
                    return QBDI_GPR_SET(&obj, index, value);
                });



}

}}
