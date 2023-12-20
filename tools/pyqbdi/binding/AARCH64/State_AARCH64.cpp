/*
 * This file is part of pyQBDI (python binding for QBDI).
 *
 * Copyright 2017 - 2023 Quarkslab
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

void hexify_register(std::ostringstream &out, __int128 v) {
  out << std::setw(32)
      << (static_cast<uint64_t>(v >> 64) & 0xffffffffffffffffull);
  out << std::setw(32) << (static_cast<uint64_t>(v) & 0xffffffffffffffffull);
}

void init_binding_State(py::module_ &m) {

  py::class_<FPRState>(m, "FPRState")
      .def(py::init<>())
      .def_readwrite("fpcr", &FPRState::fpcr, "FPCS")
      .def_readwrite("fpsr", &FPRState::fpsr, "FPSR")
      .def_property(
          "v0",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v0, sizeof(t.v0));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v0, sizeof(t.v0), 0);
          },
          "v0")
      .def_property(
          "v1",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v1, sizeof(t.v1));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v1, sizeof(t.v1), 0);
          },
          "v1")
      .def_property(
          "v2",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v2, sizeof(t.v2));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v2, sizeof(t.v2), 0);
          },
          "v2")
      .def_property(
          "v3",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v3, sizeof(t.v3));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v3, sizeof(t.v3), 0);
          },
          "v3")
      .def_property(
          "v4",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v4, sizeof(t.v4));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v4, sizeof(t.v4), 0);
          },
          "v4")
      .def_property(
          "v5",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v5, sizeof(t.v5));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v5, sizeof(t.v5), 0);
          },
          "v5")
      .def_property(
          "v6",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v6, sizeof(t.v6));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v6, sizeof(t.v6), 0);
          },
          "v6")
      .def_property(
          "v7",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v7, sizeof(t.v7));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v7, sizeof(t.v7), 0);
          },
          "v7")
      .def_property(
          "v8",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v8, sizeof(t.v8));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v8, sizeof(t.v8), 0);
          },
          "v8")
      .def_property(
          "v9",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v9, sizeof(t.v9));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v9, sizeof(t.v9), 0);
          },
          "v9")
      .def_property(
          "v10",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v10, sizeof(t.v10));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v10, sizeof(t.v10), 0);
          },
          "v10")
      .def_property(
          "v11",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v11, sizeof(t.v11));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v11, sizeof(t.v11), 0);
          },
          "v11")
      .def_property(
          "v12",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v12, sizeof(t.v12));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v12, sizeof(t.v12), 0);
          },
          "v12")
      .def_property(
          "v13",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v13, sizeof(t.v13));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v13, sizeof(t.v13), 0);
          },
          "v13")
      .def_property(
          "v14",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v14, sizeof(t.v14));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v14, sizeof(t.v14), 0);
          },
          "v14")
      .def_property(
          "v15",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v15, sizeof(t.v15));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v15, sizeof(t.v15), 0);
          },
          "v15")
      .def_property(
          "v16",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v16, sizeof(t.v16));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v16, sizeof(t.v16), 0);
          },
          "v16")
      .def_property(
          "v17",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v17, sizeof(t.v17));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v17, sizeof(t.v17), 0);
          },
          "v17")
      .def_property(
          "v18",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v18, sizeof(t.v18));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v18, sizeof(t.v18), 0);
          },
          "v18")
      .def_property(
          "v19",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v19, sizeof(t.v19));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v19, sizeof(t.v19), 0);
          },
          "v19")
      .def_property(
          "v20",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v20, sizeof(t.v20));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v20, sizeof(t.v20), 0);
          },
          "v20")
      .def_property(
          "v21",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v21, sizeof(t.v21));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v21, sizeof(t.v21), 0);
          },
          "v21")
      .def_property(
          "v22",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v22, sizeof(t.v22));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v22, sizeof(t.v22), 0);
          },
          "v22")
      .def_property(
          "v23",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v23, sizeof(t.v23));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v23, sizeof(t.v23), 0);
          },
          "v23")
      .def_property(
          "v24",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v24, sizeof(t.v24));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v24, sizeof(t.v24), 0);
          },
          "v24")
      .def_property(
          "v25",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v25, sizeof(t.v25));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v25, sizeof(t.v25), 0);
          },
          "v25")
      .def_property(
          "v26",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v26, sizeof(t.v26));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v26, sizeof(t.v26), 0);
          },
          "v26")
      .def_property(
          "v27",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v27, sizeof(t.v27));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v27, sizeof(t.v27), 0);
          },
          "v27")
      .def_property(
          "v28",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v28, sizeof(t.v28));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v28, sizeof(t.v28), 0);
          },
          "v28")
      .def_property(
          "v29",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v29, sizeof(t.v29));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v29, sizeof(t.v29), 0);
          },
          "v29")
      .def_property(
          "v30",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v30, sizeof(t.v30));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v30, sizeof(t.v30), 0);
          },
          "v30")
      .def_property(
          "v31",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.v31, sizeof(t.v31));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.v31, sizeof(t.v31), 0);
          },
          "v31")
      .def("__str__",
           [](const FPRState &obj) {
             std::ostringstream oss;
             oss << std::hex << std::setfill('0')
                 << "=== FPRState begin ===" << std::endl
                 << "fpcr  : 0x" << std::setw(sizeof(rword) * 2) << obj.fpcr
                 << std::endl
                 << "fpsr  : 0x" << std::setw(sizeof(rword) * 2) << obj.fpsr
                 << std::endl
                 << "v0  : 0x";
             hexify_register(oss, obj.v0);
             oss << std::endl << "v1  : 0x";
             hexify_register(oss, obj.v1);
             oss << std::endl << "v2  : 0x";
             hexify_register(oss, obj.v2);
             oss << std::endl << "v3  : 0x";
             hexify_register(oss, obj.v3);
             oss << std::endl << "v4  : 0x";
             hexify_register(oss, obj.v4);
             oss << std::endl << "v5  : 0x";
             hexify_register(oss, obj.v5);
             oss << std::endl << "v6  : 0x";
             hexify_register(oss, obj.v6);
             oss << std::endl << "v7  : 0x";
             hexify_register(oss, obj.v7);
             oss << std::endl << "v8  : 0x";
             hexify_register(oss, obj.v8);
             oss << std::endl << "v9  : 0x";
             hexify_register(oss, obj.v9);
             oss << std::endl << "v10 : 0x";
             hexify_register(oss, obj.v10);
             oss << std::endl << "v11 : 0x";
             hexify_register(oss, obj.v11);
             oss << std::endl << "v12 : 0x";
             hexify_register(oss, obj.v12);
             oss << std::endl << "v13 : 0x";
             hexify_register(oss, obj.v13);
             oss << std::endl << "v14 : 0x";
             hexify_register(oss, obj.v14);
             oss << std::endl << "v15 : 0x";
             hexify_register(oss, obj.v15);
             oss << std::endl << "v16 : 0x";
             hexify_register(oss, obj.v16);
             oss << std::endl << "v17 : 0x";
             hexify_register(oss, obj.v17);
             oss << std::endl << "v18 : 0x";
             hexify_register(oss, obj.v18);
             oss << std::endl << "v19 : 0x";
             hexify_register(oss, obj.v19);
             oss << std::endl << "v20 : 0x";
             hexify_register(oss, obj.v20);
             oss << std::endl << "v21 : 0x";
             hexify_register(oss, obj.v21);
             oss << std::endl << "v22 : 0x";
             hexify_register(oss, obj.v22);
             oss << std::endl << "v23 : 0x";
             hexify_register(oss, obj.v23);
             oss << std::endl << "v24 : 0x";
             hexify_register(oss, obj.v24);
             oss << std::endl << "v25 : 0x";
             hexify_register(oss, obj.v25);
             oss << std::endl << "v26 : 0x";
             hexify_register(oss, obj.v26);
             oss << std::endl << "v27 : 0x";
             hexify_register(oss, obj.v27);
             oss << std::endl << "v28 : 0x";
             hexify_register(oss, obj.v28);
             oss << std::endl << "v29 : 0x";
             hexify_register(oss, obj.v29);
             oss << std::endl << "v30 : 0x";
             hexify_register(oss, obj.v30);
             oss << std::endl << "v31 : 0x";
             hexify_register(oss, obj.v31);

             oss << std::endl << "=== FPRState end ===" << std::endl;
             return oss.str();
           })
      .def("__copy__", [](const FPRState &state) -> FPRState { return state; })
      .def(py::pickle(
          [](const FPRState &state) { // __getstate__
            return py::make_tuple(
                "AARCH64", py::bytes(reinterpret_cast<const char *>(&state),
                                     sizeof(FPRState)));
          },
          [](py::tuple t) -> FPRState { // __setstate__
            if (t.size() != 2) {
              throw std::runtime_error("Invalid state!");
            }
            if (t[0].cast<std::string>() != "AARCH64") {
              std::ostringstream oss;
              oss << "Invalid state. (expected \"AARCH64\", found \""
                  << t[0].cast<std::string>() << "\")";
              throw std::runtime_error(oss.str());
            }

            std::string buffer = t[1].cast<std::string>();

            if (buffer.size() != sizeof(FPRState)) {
              std::ostringstream oss;
              oss << "Invalid state. (expected size of " << sizeof(FPRState)
                  << ", found size of " << buffer.size() << ")";
              throw std::runtime_error(oss.str());
            }

            FPRState newState;
            memcpy(reinterpret_cast<void *>(&newState), buffer.data(),
                   sizeof(FPRState));
            return newState;

          }));

  m.attr("REG_RETURN") = REG_RETURN;
  m.attr("AVAILABLE_GPR") = AVAILABLE_GPR;
  m.attr("REG_BP") = REG_BP;
  m.attr("REG_SP") = REG_SP;
  m.attr("REG_PC") = REG_PC;
  m.attr("NUM_GPR") = NUM_GPR;
  m.attr("REG_LR") = REG_LR;
  m.attr("REG_FLAG") = REG_FLAG;

  py::class_<GPRState>(m, "GPRState")
      .def(py::init<>())
      .def_readwrite("x0", &GPRState::x0)
      .def_readwrite("x1", &GPRState::x1)
      .def_readwrite("x2", &GPRState::x2)
      .def_readwrite("x3", &GPRState::x3)
      .def_readwrite("x4", &GPRState::x4)
      .def_readwrite("x5", &GPRState::x5)
      .def_readwrite("x6", &GPRState::x6)
      .def_readwrite("x7", &GPRState::x7)
      .def_readwrite("x8", &GPRState::x8)
      .def_readwrite("x9", &GPRState::x9)
      .def_readwrite("x10", &GPRState::x10)
      .def_readwrite("x11", &GPRState::x11)
      .def_readwrite("x12", &GPRState::x12)
      .def_readwrite("x13", &GPRState::x13)
      .def_readwrite("x14", &GPRState::x14)
      .def_readwrite("x15", &GPRState::x15)
      .def_readwrite("x16", &GPRState::x16)
      .def_readwrite("x17", &GPRState::x17)
      .def_readwrite("x18", &GPRState::x18)
      .def_readwrite("x19", &GPRState::x19)
      .def_readwrite("x20", &GPRState::x20)
      .def_readwrite("x21", &GPRState::x21)
      .def_readwrite("x22", &GPRState::x22)
      .def_readwrite("x23", &GPRState::x23)
      .def_readwrite("x24", &GPRState::x24)
      .def_readwrite("x25", &GPRState::x25)
      .def_readwrite("x26", &GPRState::x26)
      .def_readwrite("x27", &GPRState::x27)
      .def_readwrite("x28", &GPRState::x28)
      .def_readwrite("x29", &GPRState::x29)
      .def_readwrite("fp", &GPRState::x29, "shadow of x29")
      .def_readwrite("lr", &GPRState::lr)
      .def_readwrite("sp", &GPRState::sp)
      .def_readwrite("nzcv", &GPRState::nzcv)
      .def_readwrite("pc", &GPRState::pc)
      .def_property(
          "localMonitor_addr",
          [](const GPRState &t) { return py::int_(t.localMonitor.addr); },
          [](GPRState &t, py::int_ v) { t.localMonitor.addr = v; },
          "localMonitor : exclusive base address")
      .def_property(
          "localMonitor_enable",
          [](const GPRState &t) {
            return py::bool_(t.localMonitor.enable != 0);
          },
          [](GPRState &t, py::bool_ v) { t.localMonitor.enable = v ? 1 : 0; },
          "localMonitor : exclusive state")
      // cross architecture access
      .def_readwrite("REG_RETURN", &GPRState::x0, "shadow of x0")
      .def_readwrite("AVAILABLE_GPR", &GPRState::x28, "shadow of x28")
      .def_readwrite("REG_BP", &GPRState::x29, "shadow of x29 / fp")
      .def_readwrite("REG_SP", &GPRState::sp, "shadow of sp")
      .def_readwrite("REG_PC", &GPRState::pc, "shadow of PC")
      .def_readwrite("NUM_GPR", &GPRState::nzcv, "shadow of nzcv")
      .def_readwrite("REG_LR", &GPRState::lr, "shadow of lr")
      .def_readwrite("REG_FLAG", &GPRState::nzcv, "shadow of nzcv")
      .def("__str__",
           [](const GPRState &obj) {
             std::ostringstream oss;
             int r = sizeof(rword) * 2;
             oss << std::hex << std::setfill('0')
                 << "=== GPRState begin ===" << std::endl
                 << "x0     : 0x" << std::setw(r) << obj.x0 << std::endl
                 << "x1     : 0x" << std::setw(r) << obj.x1 << std::endl
                 << "x2     : 0x" << std::setw(r) << obj.x2 << std::endl
                 << "x3     : 0x" << std::setw(r) << obj.x3 << std::endl
                 << "x4     : 0x" << std::setw(r) << obj.x4 << std::endl
                 << "x5     : 0x" << std::setw(r) << obj.x5 << std::endl
                 << "x6     : 0x" << std::setw(r) << obj.x6 << std::endl
                 << "x7     : 0x" << std::setw(r) << obj.x7 << std::endl
                 << "x8     : 0x" << std::setw(r) << obj.x8 << std::endl
                 << "x9     : 0x" << std::setw(r) << obj.x9 << std::endl
                 << "x10    : 0x" << std::setw(r) << obj.x10 << std::endl
                 << "x11    : 0x" << std::setw(r) << obj.x11 << std::endl
                 << "x12    : 0x" << std::setw(r) << obj.x12 << std::endl
                 << "x13    : 0x" << std::setw(r) << obj.x13 << std::endl
                 << "x14    : 0x" << std::setw(r) << obj.x14 << std::endl
                 << "x15    : 0x" << std::setw(r) << obj.x15 << std::endl
                 << "x16    : 0x" << std::setw(r) << obj.x16 << std::endl
                 << "x17    : 0x" << std::setw(r) << obj.x17 << std::endl
                 << "x18    : 0x" << std::setw(r) << obj.x18 << std::endl
                 << "x19    : 0x" << std::setw(r) << obj.x19 << std::endl
                 << "x20    : 0x" << std::setw(r) << obj.x20 << std::endl
                 << "x21    : 0x" << std::setw(r) << obj.x21 << std::endl
                 << "x22    : 0x" << std::setw(r) << obj.x22 << std::endl
                 << "x23    : 0x" << std::setw(r) << obj.x23 << std::endl
                 << "x24    : 0x" << std::setw(r) << obj.x24 << std::endl
                 << "x25    : 0x" << std::setw(r) << obj.x25 << std::endl
                 << "x26    : 0x" << std::setw(r) << obj.x26 << std::endl
                 << "x27    : 0x" << std::setw(r) << obj.x27 << std::endl
                 << "x28    : 0x" << std::setw(r) << obj.x28 << std::endl
                 << "x29|FP : 0x" << std::setw(r) << obj.x29 << std::endl
                 << "LR     : 0x" << std::setw(r) << obj.lr << std::endl
                 << "SP     : 0x" << std::setw(r) << obj.sp << std::endl
                 << "nzcv   : 0x" << std::setw(r) << obj.nzcv << std::endl
                 << "PC     : 0x" << std::setw(r) << obj.pc << std::endl
                 << "=== GPRState end ===" << std::endl;
             return oss.str();
           })
      .def(
          "__getitem__",
          [](const GPRState &obj, unsigned int index) {
            if (index >= (sizeof(GPRState) / sizeof(rword))) {
              throw pybind11::index_error("Out of range of GPRState");
            }
            return QBDI_GPR_GET(&obj, index);
          },
          "Get a register like QBDI_GPR_GET", "index"_a)
      .def(
          "__setitem__",
          [](GPRState &obj, unsigned int index, rword value) {
            if (index >= (sizeof(GPRState) / sizeof(rword))) {
              throw pybind11::index_error("Out of range of GPRState");
            }
            return QBDI_GPR_SET(&obj, index, value);
          },
          "Set a register like QBDI_GPR_SET", "index"_a, "value"_a)
      .def("__copy__", [](const GPRState &state) -> GPRState { return state; })
      .def(py::pickle(
          [](const GPRState &state) { // __getstate__
            return py::make_tuple(
                "AARCH64", py::bytes(reinterpret_cast<const char *>(&state),
                                     sizeof(GPRState)));
          },
          [](py::tuple t) -> GPRState { // __setstate__
            if (t.size() != 2) {
              throw std::runtime_error("Invalid state!");
            }
            if (t[0].cast<std::string>() != "AARCH64") {
              std::ostringstream oss;
              oss << "Invalid state. (expected \"AARCH64\", found \""
                  << t[0].cast<std::string>() << "\")";
              throw std::runtime_error(oss.str());
            }

            std::string buffer = t[1].cast<std::string>();

            if (buffer.size() != sizeof(GPRState)) {
              std::ostringstream oss;
              oss << "Invalid state. (expected size of " << sizeof(GPRState)
                  << ", found size of " << buffer.size() << ")";
              throw std::runtime_error(oss.str());
            }

            GPRState newState;
            memcpy(reinterpret_cast<void *>(&newState), buffer.data(),
                   sizeof(GPRState));
            return newState;

          }));
}

} // namespace pyQBDI
} // namespace QBDI
