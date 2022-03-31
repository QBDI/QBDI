/*
 * This file is part of pyQBDI (python binding for QBDI).
 *
 * Copyright 2017 - 2022 Quarkslab
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

#ifndef PYQBDI_ENUM_HPP_
#define PYQBDI_ENUM_HPP_

#include <map>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>

namespace py = pybind11;

namespace QBDI {
namespace pyQBDI {

template <class Type>
class enum_int_flag_ : public pybind11::enum_<Type> {
public:
  using py::enum_<Type>::def;
  using py::enum_<Type>::def_property_readonly_static;
  using Scalar = typename py::enum_<Type>::Scalar;

private:
  std::string enum_name;
  std::map<Scalar, std::string> members_names;
  Scalar mask;
  bool is_arithmetic;

public:
  template <typename... Extra>
  enum_int_flag_(const py::handle &scope, const char *name,
                 const Extra &...extra)
      : py::enum_<Type>{scope, name, extra...}, enum_name(name), mask(0) {
    constexpr bool is_arithmetic_ =
        py::detail::any_of<std::is_same<py::arithmetic, Extra>...>::value;
    is_arithmetic = is_arithmetic_;
    def("__eq__",
        [](const Type &value, const Type &value2) { return value == value2; });
    def("__ne__",
        [](const Type &value, const Type &value2) { return value != value2; });
    if (is_arithmetic_) {
      def(
          "__and__",
          [](const Type &value, const Type &value2) {
            return static_cast<Type>(value & value2);
          },
          py::prepend());
      def(
          "__or__",
          [](const Type &value, const Type &value2) {
            return static_cast<Type>(value | value2);
          },
          py::prepend());
      def(
          "__xor__",
          [](const Type &value, const Type &value2) {
            return static_cast<Type>(value ^ value2);
          },
          py::prepend());
    }
  }

  enum_int_flag_ &value(char const *name, Type value,
                        const char *doc = nullptr) {
    members_names[(Scalar)value] = std::string(name);
    mask |= (Scalar)value;
    pybind11::enum_<Type>::value(name, value, doc);
    return *this;
  }

  enum_int_flag_ &export_values() {
    pybind11::enum_<Type>::export_values();
    return *this;
  }

  enum_int_flag_ &def_invert() {
    if (is_arithmetic) {
      def(
          "__invert__",
          [m = mask](const Type &value) {
            return static_cast<Type>(value ^ m);
          },
          py::prepend());
    }
    return *this;
  }

  enum_int_flag_ &def_repr_str() {
    def(
        "__str__",
        [enum_name = enum_name,
         members_names = members_names](const Type &value) {
          return py::str("{}.{}").format(
              enum_name,
              enum_int_flag_<Type>::get_member_name(members_names, value));
        },
        py::prepend());
    def(
        "__repr__",
        [enum_name = enum_name,
         members_names = members_names](const Type &value) {
          return py::str("<{}.{}: {}>")
              .format(
                  enum_name,
                  enum_int_flag_<Type>::get_member_name(members_names, value),
                  (Scalar)value);
        },
        py::prepend());

    return *this;
  }

  static inline std::string
  get_member_name(const std::map<Scalar, std::string> &members_names,
                  Scalar v) {
    const auto it = members_names.find(v);
    if (it != members_names.end()) {
      return it->second;
    }
    if (v == 0) {
      return "0";
    }
    std::string res;
    for (unsigned int i = 0; i < sizeof(Scalar) * 8 && v != 0; i++) {
      if ((v & (1 << i)) != 0) {
        const auto it = members_names.find(1 << i);
        if (it != members_names.end()) {
          if (res.size() != 0) {
            res += "|";
          }
          res += it->second;
        }
        v ^= (1 << i);
      }
    }
    if (res.size() == 0) {
      return "???";
    }
    return res;
  }
};
} // namespace pyQBDI
} // namespace QBDI

#endif
