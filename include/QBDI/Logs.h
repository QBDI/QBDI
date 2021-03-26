/*
 * This file is part of QBDI.
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
#ifndef QBDI_LOGS_H_
#define QBDI_LOGS_H_

#include <stdio.h>

#include "Platform.h"

#ifdef __cplusplus
namespace QBDI {
extern "C" {
#endif

/*! Each log has a priority (or level) which can be used to control verbosity.
 * In production builds, only Warning and Error logs are kept.
 */
typedef enum {
    _QBDI_EI(DEBUG) = 0, /*!< Debug logs */
    _QBDI_EI(WARNING),   /*!< Warning logs */
    _QBDI_EI(ERROR),     /*!< Error logs */
} LogPriority;


/*! Redirect logs to an opened file.
 *
 * @param[in] output        Pointer to an opened file where logs will be append.
 */
QBDI_EXPORT void qbdi_setLogOutput(FILE* output);

/*! Enable logs matching tag and priority.
 *
 * @param[in] tag           Logs are identified using a tag (ex: Engine::patch).
 *                          "*" will log everything matching priority.
 * @param[in] priority      Filter logs with greater or equal priority.
 */
QBDI_EXPORT void qbdi_addLogFilter(const char *tag, LogPriority priority);

#ifdef __cplusplus
/*
 * C API C++ bindings
 */

/*! Redirect logs to an opened file.
 *
 * @param[in] output        Pointer to an opened file where logs will be append.
 */
inline void setLogOutput(FILE* output) {
    return qbdi_setLogOutput(output);
}

/*! Enable logs matching tag and priority.
 *
 * @param[in] tag           Logs are identified using a tag (ex: Engine::patch).
 *                          "*" will log everything matching priority.
 * @param[in] priority      Filter logs with greater or equal priority.
 */
inline void addLogFilter(const char *tag, LogPriority priority) {
    return qbdi_addLogFilter(tag, priority);
}

} // "C"
} // QBDI::
#endif

#endif // QBDI_LOGS_H_
