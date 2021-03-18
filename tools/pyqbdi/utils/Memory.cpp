/*
 * This file is part of pyQBDI (python binding for QBDI).
 *
 * Copyright 2017 - 2021 Quarkslab
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

void init_utils_Memory(py::module_& m) {

    m.def("readMemory",
            [](QBDI::rword address, QBDI::rword size) {
                return py::bytes(reinterpret_cast<const char*>(address), size);
            }, R"doc(
            Read a content from a base address.

            :param address: Base address
            :param size: Read size

            :returns: Bytes of content.

            .. warning::
                This API is hazardous as the whole process memory can be read.
            )doc",
            "address"_a, "size"_a);

    m.def("readRword",
            [](QBDI::rword address) {
                return *(reinterpret_cast<QBDI::rword*>(address));
            }, R"doc(
            Read a rword to the specified address

            :param address: Base address

            :returns: the value as a unsigned integer

            .. warning::
                This API is hazardous as the whole process memory can be read.
            )doc",
            "address"_a);

    m.def("writeMemory",
            [](QBDI::rword address, std::string bytes) {
                memcpy(reinterpret_cast<void*>(address), bytes.c_str(), bytes.size());
            }, R"doc(
            Write a memory content to a base address.

            :param address: Base address
            :param bytes: Memory content

            .. warning::
                This API is hazardous as the whole process memory can be written.
            )doc",
            "address"_a, "bytes"_a);

    m.def("writeRword",
            [](QBDI::rword address, QBDI::rword value) {
                *(reinterpret_cast<QBDI::rword*>(address)) = value;
            }, R"doc(
            Write a rword in a base address.

            :param address: Base address
            :param value: The value to write, as a unsigned integer

            .. warning::
                This API is hazardous as the whole process memory can be written.
            )doc",
            "address"_a, "value"_a);

    m.def("allocateRword",
            []() {
                QBDI::rword addr = reinterpret_cast<QBDI::rword>( std::malloc(sizeof(rword)));
                if (addr == 0) {
                    throw std::bad_alloc{};
                }
                return addr;
            }, R"doc(
            Allocate a raw memory space to store a rword.

            :returns: Address to a memory space to store a rword
            )doc");

    m.def("allocateMemory",
            [](QBDI::rword length) {
                QBDI::rword addr = reinterpret_cast<QBDI::rword>( std::malloc(length));
                if (addr == 0) {
                    throw std::bad_alloc{};
                }
                return addr;
            }, R"doc(
            Allocate a raw memory space of specified length.

            :param length: length of the memory space to allocate

            :returns: Address to the allocated memory
            )doc",
            "length"_a);

    m.def("freeMemory",
            [](QBDI::rword address) {
                std::free( reinterpret_cast<void*>(address));
            }, R"doc(
            Free a memory space allocate with allocateRword or allocateMemory.

            :param address: Address of the allocated memory
            )doc",
            "address"_a);
}

}
}
