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

void init_binding_Logs(py::module& m) {

    py::enum_<LogPriority>(m, "LogPriority",
            "Each log has a priority (or level) which can be used to control verbosity\n"
            "In production builds, only Warning and Error logs are kept.")
        .value("DEBUG", LogPriority::DEBUG, "Debug logs")
        .value("WARNING", LogPriority::WARNING, "Warning logs")
        .value("ERROR", LogPriority::ERROR, "Error logs")
        .export_values();

    m.def("addLogFilter", &addLogFilter,
            "Enable logs matching tag and priority.",
            "tag"_a, "priority"_a);

}

}}
