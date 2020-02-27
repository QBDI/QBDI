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

void init_binding_Range(py::module& m) {
    py::class_<Range<rword>>(m, "Range")
        .def(py::init<rword, rword>(),
                "Create a new range",
                "start"_a, "end"_a)

        .def_readwrite("start", &Range<rword>::start,
                "Range start value.")

        .def_readwrite("end", &Range<rword>::end,
                "Range end value (always excluded).")

        .def("size", &Range<rword>::size,
                "Return the total length of a range.")

        .def(py::self == py::self,
                "Return True if two ranges are equal (same boundaries).",
                "r"_a)

        .def("contains", (bool (Range<rword>::*)(rword) const) &Range<rword>::contains,
                "Return True if an value is inside current range boundaries.",
                "t"_a)

        .def("contains", (bool (Range<rword>::*)(Range<rword>) const) &Range<rword>::contains,
                "Return True if a range is inside current range boundaries.",
                "r"_a)

        .def("__contains__", (bool (Range<rword>::*)(rword) const) &Range<rword>::contains,
                "Return True if an value is inside current range boundaries.",
                "t"_a)

        .def("__contains__", (bool (Range<rword>::*)(Range<rword>) const) &Range<rword>::contains,
                "Return True if a range is inside current range boundaries.",
                "r"_a)

        .def("overlaps", &Range<rword>::overlaps,
                "Return True if a range is overlapping current range lower or/and upper boundary.",
                "r"_a)

        .def("__str__", [](const Range<rword> &r) {
                               std::ostringstream oss;
                               r.display(oss);
                               return oss.str();
                           })
        .def("__repr__", [](const Range<rword> &r) {
                               std::ostringstream oss;
                               r.display(oss);
                               return "<Range " + oss.str() + ">";
                           })
        .def("intersect", &Range<rword>::intersect,
                "Return the intersection of two ranges.",
                "r"_a)
        .def("__getitem__", [](const Range<rword> &r, int index) {
                                switch (index) {
                                    case 0:
                                        return r.start;
                                    case 1:
                                        return r.end;
                                    default:
                                        throw std::out_of_range("Only two elements");
                                }
                            })
        .def("__setitem__", [](Range<rword> &r, int index, rword value) {
                                switch (index) {
                                    case 0:
                                        r.start = value;
                                    case 1:
                                        r.end = value;
                                    default:
                                        throw std::out_of_range("Only two elements");
                                }
                            });
}

}
}
