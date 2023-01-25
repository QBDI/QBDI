/*
 * This file is part of QBDI.
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
#ifndef QBDI_LOGS_H_
#define QBDI_LOGS_H_

#include <stdio.h>
#include "QBDI/Platform.h"

#ifdef __cplusplus
#include <string>
#endif

#ifdef __cplusplus
namespace QBDI {
extern "C" {
#endif

/*! Each log has a priority (or level) which can be used to control verbosity.
 * In production builds, only Warning and Error logs are kept.
 */
typedef enum {
  _QBDI_EI(DEBUG) = 0,      /*!< Debug logs */
  _QBDI_EI(INFO),           /*!< Info logs (default) */
  _QBDI_EI(WARNING),        /*!< Warning logs */
  _QBDI_EI(ERROR),          /*!< Error logs */
  _QBDI_EI(DISABLE) = 0xff, /*!< Disable logs message */
} LogPriority;

/*! Redirect logs to a file.
 *
 * @param[in] filename    the path of the file to append the log
 * @param[in] truncate    Set to true to clear the file before append the log
 */
QBDI_EXPORT void qbdi_setLogFile(const char *filename, bool truncate);

/*! Write log to the console (stderr)
 */
QBDI_EXPORT void qbdi_setLogConsole();

/*! Write log to the default location (stderr for linux, android_logger for
 * android)
 */
QBDI_EXPORT void qbdi_setLogDefault();

/*! Enable logs matching priority.
 *
 * @param[in] priority      Filter logs with greater or equal priority.
 */
QBDI_EXPORT void qbdi_setLogPriority(LogPriority priority);

#ifdef __cplusplus

/*
 * C API C++ bindings
 */

/*! Redirect logs to a file.
 *
 * @param[in] filename    the path of the file to append the log
 * @param[in] truncate    Set to true to clear the file before append the log
 */
QBDI_EXPORT void setLogFile(const std::string &filename, bool truncate = false);

/*! Enable logs matching priority.
 *
 * @param[in] priority      Filter logs with greater or equal priority.
 */
inline void setLogPriority(LogPriority priority = LogPriority::INFO) {
  return qbdi_setLogPriority(priority);
}

/*! Write log to the console (stderr)
 */
inline void setLogConsole() { return qbdi_setLogConsole(); }

/*! Write log to the default location (stderr for linux, android_logger for
 * android)
 */
inline void setLogDefault() { return qbdi_setLogDefault(); }

} // "C"

} // QBDI::
#endif

#endif // QBDI_LOGS_H_
