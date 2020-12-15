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

void init_utils_Float(py::module_& m) {

    m.def("encodeFloat",
            [](float val) {
                union {float f; int32_t i;} t;
                t.f = val;
                return t.i;
            }, R"doc(
            Encode a float as a signed integer.

            :param val: Float value

            :returns: a sigend integer
            )doc",
            "val"_a);

    m.def("encodeFloatU",
            [](float val) {
                union {float f; uint32_t i;} t;
                t.f = val;
                return t.i;
            }, R"doc(
            Encode a float as an unsigned interger.

            :param val: Float value

            :returns: an unsigned integer
            )doc",
            "val"_a);

    m.def("decodeFloat",
            [](int32_t val) {
                union {float f; int32_t i;} t;
                t.i = val;
                return t.f;
            }, R"doc(
            Encode a sigend integer as a float.

            :param val: signed integer value

            :returns: a float
            )doc",
            "val"_a);

    m.def("decodeFloatU",
            [](uint32_t val) {
                union {float f; uint32_t i;} t;
                t.i = val;
                return t.f;
            }, R"doc(
            Encode an unsigend integer as a float.

            :param val: unsigned integer value

            :returns: a float
            )doc",
            "val"_a);

    m.def("encodeDouble",
            [](double val) {
                union {double f; int64_t i;} t;
                t.f = val;
                return t.i;
            }, R"doc(
            Encode a double as a signed integer.

            :param val: Double value

            :returns: a sigend integer
            )doc",
            "val"_a);

    m.def("encodeDoubleU",
            [](double val) {
                union {double f; uint64_t i;} t;
                t.f = val;
                return t.i;
            }, R"doc(
            Encode a double as an unsigned interger.

            :param val: Double value

            :returns: an unsigned integer
            )doc",
            "val"_a);

    m.def("decodeDouble",
            [](int64_t val) {
                union {double f; int64_t i;} t;
                t.i = val;
                return t.f;
            }, R"doc(
            Encode a sigend integer as a double.

            :param val: signed integer value

            :returns: a double
            )doc",
            "val"_a);

    m.def("decodeDoubleU",
            [](uint64_t val) {
                union {double f; uint64_t i;} t;
                t.i = val;
                return t.f;
            }, R"doc(
            Encode an unsigend integer as a double.

            :param val: unsigned integer value

            :returns: a double
            )doc",
            "val"_a);
}

}
}
