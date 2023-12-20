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

void init_binding_State(py::module_ &m) {

  py::class_<FPRState>(m, "FPRState")
      .def(py::init<>())
      .def_readwrite("fpscr", &FPRState::fpscr, "FPSCR")
      .def_property(
          "s0",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[0], sizeof(t.vreg.s[0]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[0], sizeof(t.vreg.s[0]), 0);
          },
          "s0")
      .def_property(
          "s1",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[1], sizeof(t.vreg.s[1]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[1], sizeof(t.vreg.s[1]), 0);
          },
          "s1")
      .def_property(
          "s2",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[2], sizeof(t.vreg.s[2]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[2], sizeof(t.vreg.s[2]), 0);
          },
          "s2")
      .def_property(
          "s3",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[3], sizeof(t.vreg.s[3]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[3], sizeof(t.vreg.s[3]), 0);
          },
          "s3")
      .def_property(
          "s4",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[4], sizeof(t.vreg.s[4]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[4], sizeof(t.vreg.s[4]), 0);
          },
          "s4")
      .def_property(
          "s5",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[5], sizeof(t.vreg.s[5]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[5], sizeof(t.vreg.s[5]), 0);
          },
          "s5")
      .def_property(
          "s6",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[6], sizeof(t.vreg.s[6]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[6], sizeof(t.vreg.s[6]), 0);
          },
          "s6")
      .def_property(
          "s7",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[7], sizeof(t.vreg.s[7]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[7], sizeof(t.vreg.s[7]), 0);
          },
          "s7")
      .def_property(
          "s8",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[8], sizeof(t.vreg.s[8]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[8], sizeof(t.vreg.s[8]), 0);
          },
          "s8")
      .def_property(
          "s9",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[9], sizeof(t.vreg.s[9]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[9], sizeof(t.vreg.s[9]), 0);
          },
          "s9")
      .def_property(
          "s10",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[10], sizeof(t.vreg.s[10]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[10], sizeof(t.vreg.s[10]), 0);
          },
          "s10")
      .def_property(
          "s11",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[11], sizeof(t.vreg.s[11]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[11], sizeof(t.vreg.s[11]), 0);
          },
          "s11")
      .def_property(
          "s12",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[12], sizeof(t.vreg.s[12]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[12], sizeof(t.vreg.s[12]), 0);
          },
          "s12")
      .def_property(
          "s13",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[13], sizeof(t.vreg.s[13]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[13], sizeof(t.vreg.s[13]), 0);
          },
          "s13")
      .def_property(
          "s14",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[14], sizeof(t.vreg.s[14]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[14], sizeof(t.vreg.s[14]), 0);
          },
          "s14")
      .def_property(
          "s15",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[15], sizeof(t.vreg.s[15]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[15], sizeof(t.vreg.s[15]), 0);
          },
          "s15")
      .def_property(
          "s16",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[16], sizeof(t.vreg.s[16]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[16], sizeof(t.vreg.s[16]), 0);
          },
          "s16")
      .def_property(
          "s17",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[17], sizeof(t.vreg.s[17]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[17], sizeof(t.vreg.s[17]), 0);
          },
          "s17")
      .def_property(
          "s18",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[18], sizeof(t.vreg.s[18]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[18], sizeof(t.vreg.s[18]), 0);
          },
          "s18")
      .def_property(
          "s19",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[19], sizeof(t.vreg.s[19]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[19], sizeof(t.vreg.s[19]), 0);
          },
          "s19")
      .def_property(
          "s20",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[20], sizeof(t.vreg.s[20]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[20], sizeof(t.vreg.s[20]), 0);
          },
          "s20")
      .def_property(
          "s21",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[21], sizeof(t.vreg.s[21]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[21], sizeof(t.vreg.s[21]), 0);
          },
          "s21")
      .def_property(
          "s22",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[22], sizeof(t.vreg.s[22]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[22], sizeof(t.vreg.s[22]), 0);
          },
          "s22")
      .def_property(
          "s23",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[23], sizeof(t.vreg.s[23]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[23], sizeof(t.vreg.s[23]), 0);
          },
          "s23")
      .def_property(
          "s24",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[24], sizeof(t.vreg.s[24]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[24], sizeof(t.vreg.s[24]), 0);
          },
          "s24")
      .def_property(
          "s25",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[25], sizeof(t.vreg.s[25]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[25], sizeof(t.vreg.s[25]), 0);
          },
          "s25")
      .def_property(
          "s26",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[26], sizeof(t.vreg.s[26]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[26], sizeof(t.vreg.s[26]), 0);
          },
          "s26")
      .def_property(
          "s27",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[27], sizeof(t.vreg.s[27]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[27], sizeof(t.vreg.s[27]), 0);
          },
          "s27")
      .def_property(
          "s28",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[28], sizeof(t.vreg.s[28]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[28], sizeof(t.vreg.s[28]), 0);
          },
          "s28")
      .def_property(
          "s29",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[29], sizeof(t.vreg.s[29]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[29], sizeof(t.vreg.s[29]), 0);
          },
          "s29")
      .def_property(
          "s30",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[20], sizeof(t.vreg.s[20]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[20], sizeof(t.vreg.s[20]), 0);
          },
          "s30")
      .def_property(
          "s31",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.s[31], sizeof(t.vreg.s[31]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.s[31], sizeof(t.vreg.s[31]), 0);
          },
          "s31")
      .def_property(
          "d0",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[0], sizeof(t.vreg.d[0]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[0], sizeof(t.vreg.d[0]), 0);
          },
          "d0")
      .def_property(
          "d1",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[1], sizeof(t.vreg.d[1]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[1], sizeof(t.vreg.d[1]), 0);
          },
          "d1")
      .def_property(
          "d2",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[2], sizeof(t.vreg.d[2]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[2], sizeof(t.vreg.d[2]), 0);
          },
          "d2")
      .def_property(
          "d3",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[3], sizeof(t.vreg.d[3]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[3], sizeof(t.vreg.d[3]), 0);
          },
          "d3")
      .def_property(
          "d4",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[4], sizeof(t.vreg.d[4]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[4], sizeof(t.vreg.d[4]), 0);
          },
          "d4")
      .def_property(
          "d5",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[5], sizeof(t.vreg.d[5]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[5], sizeof(t.vreg.d[5]), 0);
          },
          "d5")
      .def_property(
          "d6",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[6], sizeof(t.vreg.d[6]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[6], sizeof(t.vreg.d[6]), 0);
          },
          "d6")
      .def_property(
          "d7",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[7], sizeof(t.vreg.d[7]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[7], sizeof(t.vreg.d[7]), 0);
          },
          "d7")
      .def_property(
          "d8",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[8], sizeof(t.vreg.d[8]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[8], sizeof(t.vreg.d[8]), 0);
          },
          "d8")
      .def_property(
          "d9",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[9], sizeof(t.vreg.d[9]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[9], sizeof(t.vreg.d[9]), 0);
          },
          "d9")
      .def_property(
          "d10",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[10], sizeof(t.vreg.d[10]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[10], sizeof(t.vreg.d[10]), 0);
          },
          "d10")
      .def_property(
          "d11",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[11], sizeof(t.vreg.d[11]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[11], sizeof(t.vreg.d[11]), 0);
          },
          "d11")
      .def_property(
          "d12",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[12], sizeof(t.vreg.d[12]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[12], sizeof(t.vreg.d[12]), 0);
          },
          "d12")
      .def_property(
          "d13",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[13], sizeof(t.vreg.d[13]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[13], sizeof(t.vreg.d[13]), 0);
          },
          "d13")
      .def_property(
          "d14",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[14], sizeof(t.vreg.d[14]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[14], sizeof(t.vreg.d[14]), 0);
          },
          "d14")
      .def_property(
          "d15",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[15], sizeof(t.vreg.d[15]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[15], sizeof(t.vreg.d[15]), 0);
          },
          "d15")
#if QBDI_NUM_FPR == 32
      .def_property(
          "d16",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[16], sizeof(t.vreg.d[16]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[16], sizeof(t.vreg.d[16]), 0);
          },
          "d16")
      .def_property(
          "d17",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[17], sizeof(t.vreg.d[17]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[17], sizeof(t.vreg.d[17]), 0);
          },
          "d17")
      .def_property(
          "d18",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[18], sizeof(t.vreg.d[18]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[18], sizeof(t.vreg.d[18]), 0);
          },
          "d18")
      .def_property(
          "d19",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[19], sizeof(t.vreg.d[19]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[19], sizeof(t.vreg.d[19]), 0);
          },
          "d19")
      .def_property(
          "d20",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[20], sizeof(t.vreg.d[20]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[20], sizeof(t.vreg.d[20]), 0);
          },
          "d20")
      .def_property(
          "d21",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[21], sizeof(t.vreg.d[21]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[21], sizeof(t.vreg.d[21]), 0);
          },
          "d21")
      .def_property(
          "d22",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[22], sizeof(t.vreg.d[22]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[22], sizeof(t.vreg.d[22]), 0);
          },
          "d22")
      .def_property(
          "d23",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[23], sizeof(t.vreg.d[23]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[23], sizeof(t.vreg.d[23]), 0);
          },
          "d23")
      .def_property(
          "d24",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[24], sizeof(t.vreg.d[24]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[24], sizeof(t.vreg.d[24]), 0);
          },
          "d24")
      .def_property(
          "d25",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[25], sizeof(t.vreg.d[25]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[25], sizeof(t.vreg.d[25]), 0);
          },
          "d25")
      .def_property(
          "d26",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[26], sizeof(t.vreg.d[26]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[26], sizeof(t.vreg.d[26]), 0);
          },
          "d26")
      .def_property(
          "d27",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[27], sizeof(t.vreg.d[27]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[27], sizeof(t.vreg.d[27]), 0);
          },
          "d27")
      .def_property(
          "d28",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[28], sizeof(t.vreg.d[28]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[28], sizeof(t.vreg.d[28]), 0);
          },
          "d28")
      .def_property(
          "d29",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[29], sizeof(t.vreg.d[29]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[29], sizeof(t.vreg.d[29]), 0);
          },
          "d29")
      .def_property(
          "d30",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[20], sizeof(t.vreg.d[20]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[20], sizeof(t.vreg.d[20]), 0);
          },
          "d30")
      .def_property(
          "d31",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.d[31], sizeof(t.vreg.d[31]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.d[31], sizeof(t.vreg.d[31]), 0);
          },
          "d31")
#endif
      .def_property(
          "q0",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.q[0], sizeof(t.vreg.q[0]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.q[0], sizeof(t.vreg.q[0]), 0);
          },
          "q0")
      .def_property(
          "q1",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.q[1], sizeof(t.vreg.q[1]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.q[1], sizeof(t.vreg.q[1]), 0);
          },
          "q1")
      .def_property(
          "q2",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.q[2], sizeof(t.vreg.q[2]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.q[2], sizeof(t.vreg.q[2]), 0);
          },
          "q2")
      .def_property(
          "q3",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.q[3], sizeof(t.vreg.q[3]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.q[3], sizeof(t.vreg.q[3]), 0);
          },
          "q3")
      .def_property(
          "q4",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.q[4], sizeof(t.vreg.q[4]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.q[4], sizeof(t.vreg.q[4]), 0);
          },
          "q4")
      .def_property(
          "q5",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.q[5], sizeof(t.vreg.q[5]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.q[5], sizeof(t.vreg.q[5]), 0);
          },
          "q5")
      .def_property(
          "q6",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.q[6], sizeof(t.vreg.q[6]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.q[6], sizeof(t.vreg.q[6]), 0);
          },
          "q6")
      .def_property(
          "q7",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.q[7], sizeof(t.vreg.q[7]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.q[7], sizeof(t.vreg.q[7]), 0);
          },
          "q7")
#if QBDI_NUM_FPR == 32
      .def_property(
          "q8",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.q[8], sizeof(t.vreg.q[8]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.q[8], sizeof(t.vreg.q[8]), 0);
          },
          "q8")
      .def_property(
          "q9",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.q[9], sizeof(t.vreg.q[9]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.q[9], sizeof(t.vreg.q[9]), 0);
          },
          "q9")
      .def_property(
          "q10",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.q[10], sizeof(t.vreg.q[10]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.q[10], sizeof(t.vreg.q[10]), 0);
          },
          "q10")
      .def_property(
          "q11",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.q[11], sizeof(t.vreg.q[11]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.q[11], sizeof(t.vreg.q[11]), 0);
          },
          "q11")
      .def_property(
          "q12",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.q[12], sizeof(t.vreg.q[12]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.q[12], sizeof(t.vreg.q[12]), 0);
          },
          "q12")
      .def_property(
          "q13",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.q[13], sizeof(t.vreg.q[13]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.q[13], sizeof(t.vreg.q[13]), 0);
          },
          "q13")
      .def_property(
          "q14",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.q[14], sizeof(t.vreg.q[14]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.q[14], sizeof(t.vreg.q[14]), 0);
          },
          "q14")
      .def_property(
          "q15",
          [](const FPRState &t) {
            return py::bytes((const char *)&t.vreg.q[15], sizeof(t.vreg.q[15]));
          },
          [](FPRState &t, py::bytes v) {
            std::string(v).copy((char *)&t.vreg.q[15], sizeof(t.vreg.q[15]), 0);
          },
          "q15")
#endif
      .def("__str__",
           [](const FPRState &obj) {
             std::ostringstream oss;
             oss << std::hex << std::setfill('0')
                 << "=== FPRState begin ===" << std::endl
                 << "fpscr  : 0x" << std::setw(sizeof(rword) * 2) << obj.fpscr
                 << std::endl
                 << "d0  : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[0] << std::endl
                 << "d1  : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[1] << std::endl
                 << "d2  : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[2] << std::endl
                 << "d3  : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[3] << std::endl
                 << "d4  : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[4] << std::endl
                 << "d5  : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[5] << std::endl
                 << "d6  : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[6] << std::endl
                 << "d7  : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[7] << std::endl
                 << "d8  : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[8] << std::endl
                 << "d9  : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[9] << std::endl
                 << "d10 : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[10] << std::endl
                 << "d11 : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[11] << std::endl
                 << "d12 : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[12] << std::endl
                 << "d13 : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[13] << std::endl
                 << "d14 : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[14] << std::endl
                 << "d15 : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[15] << std::endl
#if QBDI_NUM_FPR == 32
                 << "d16 : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[16] << std::endl
                 << "d17 : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[17] << std::endl
                 << "d18 : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[18] << std::endl
                 << "d19 : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[19] << std::endl
                 << "d20 : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[20] << std::endl
                 << "d21 : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[21] << std::endl
                 << "d22 : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[22] << std::endl
                 << "d23 : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[23] << std::endl
                 << "d24 : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[24] << std::endl
                 << "d25 : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[25] << std::endl
                 << "d26 : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[26] << std::endl
                 << "d27 : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[27] << std::endl
                 << "d28 : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[28] << std::endl
                 << "d29 : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[29] << std::endl
                 << "d30 : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[30] << std::endl
                 << "d31 : 0x" << std::setw(sizeof(uint64_t) * 2)
                 << (uint64_t)obj.vreg.d[31] << std::endl
#endif
                 << "=== FPRState end ===" << std::endl;
             return oss.str();
           })
      .def("__copy__", [](const FPRState &state) -> FPRState { return state; })
      .def(py::pickle(
          [](const FPRState &state) { // __getstate__
            return py::make_tuple(
                "ARM", py::bytes(reinterpret_cast<const char *>(&state),
                                 sizeof(FPRState)));
          },
          [](py::tuple t) -> FPRState { // __setstate__
            if (t.size() != 2) {
              throw std::runtime_error("Invalid state!");
            }
            if (t[0].cast<std::string>() != "ARM") {
              std::ostringstream oss;
              oss << "Invalid state. (expected \"ARM\", found \""
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
      .def_readwrite("r0", &GPRState::r0)
      .def_readwrite("r1", &GPRState::r1)
      .def_readwrite("r2", &GPRState::r2)
      .def_readwrite("r3", &GPRState::r3)
      .def_readwrite("r4", &GPRState::r4)
      .def_readwrite("r5", &GPRState::r5)
      .def_readwrite("r6", &GPRState::r6)
      .def_readwrite("r7", &GPRState::r7)
      .def_readwrite("r8", &GPRState::r8)
      .def_readwrite("r9", &GPRState::r9)
      .def_readwrite("r10", &GPRState::r10)
      .def_readwrite("r11", &GPRState::r11)
      .def_readwrite("r12", &GPRState::r12)
      .def_readwrite("fp", &GPRState::r12, "shadow of r12")
      .def_readwrite("sp", &GPRState::sp)
      .def_readwrite("lr", &GPRState::lr)
      .def_readwrite("pc", &GPRState::pc)
      .def_readwrite("cpsr", &GPRState::cpsr)
      .def_property(
          "localMonitor_addr",
          [](const GPRState &t) { return py::int_(t.localMonitor.addr); },
          [](GPRState &t, py::int_ v) { t.localMonitor.addr = v; },
          "localMonitor : exclusive base address")
      .def_property(
          "localMonitor_enable",
          [](const GPRState &t) { return py::int_(t.localMonitor.enable); },
          [](GPRState &t, py::int_ v) { t.localMonitor.enable = v; },
          "localMonitor : exclusive state")
      // cross architecture access
      .def_readwrite("REG_RETURN", &GPRState::r0, "shadow of r0")
      .def_readwrite("AVAILABLE_GPR", &GPRState::sp, "shadow of sp")
      .def_readwrite("REG_BP", &GPRState::r12, "shadow of r12 / r12")
      .def_readwrite("REG_SP", &GPRState::sp, "shadow of sp")
      .def_readwrite("REG_PC", &GPRState::pc, "shadow of PC")
      .def_readwrite("NUM_GPR", &GPRState::cpsr, "shadow of cpsr")
      .def_readwrite("REG_LR", &GPRState::lr, "shadow of lr")
      .def_readwrite("REG_FLAG", &GPRState::cpsr, "shadow of cpsr")
      .def("__str__",
           [](const GPRState &obj) {
             std::ostringstream oss;
             int r = sizeof(rword) * 2;
             oss << std::hex << std::setfill('0')
                 << "=== GPRState begin ===" << std::endl
                 << "r0     : 0x" << std::setw(r) << obj.r0 << std::endl
                 << "r1     : 0x" << std::setw(r) << obj.r1 << std::endl
                 << "r2     : 0x" << std::setw(r) << obj.r2 << std::endl
                 << "r3     : 0x" << std::setw(r) << obj.r3 << std::endl
                 << "r4     : 0x" << std::setw(r) << obj.r4 << std::endl
                 << "r5     : 0x" << std::setw(r) << obj.r5 << std::endl
                 << "r6     : 0x" << std::setw(r) << obj.r6 << std::endl
                 << "r7     : 0x" << std::setw(r) << obj.r7 << std::endl
                 << "r8     : 0x" << std::setw(r) << obj.r8 << std::endl
                 << "r9     : 0x" << std::setw(r) << obj.r9 << std::endl
                 << "r10    : 0x" << std::setw(r) << obj.r10 << std::endl
                 << "r11    : 0x" << std::setw(r) << obj.r11 << std::endl
                 << "r12|FP : 0x" << std::setw(r) << obj.r12 << std::endl
                 << "SP     : 0x" << std::setw(r) << obj.sp << std::endl
                 << "LR     : 0x" << std::setw(r) << obj.lr << std::endl
                 << "PC     : 0x" << std::setw(r) << obj.pc << std::endl
                 << "cpsr   : 0x" << std::setw(r) << obj.cpsr << std::endl
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
                "ARM", py::bytes(reinterpret_cast<const char *>(&state),
                                 sizeof(GPRState)));
          },
          [](py::tuple t) -> GPRState { // __setstate__
            if (t.size() != 2) {
              throw std::runtime_error("Invalid state!");
            }
            if (t[0].cast<std::string>() != "ARM") {
              std::ostringstream oss;
              oss << "Invalid state. (expected \"ARM\", found \""
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
