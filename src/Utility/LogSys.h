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
#ifndef LOGSYS_H
#define LOGSYS_H

#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <vector>
#include <functional>
#include <utility>

#include "Platform.h"
#include "Logs.h"

#if defined(QBDI_PLATFORM_LINUX) || defined(QBDI_PLATFORM_ANDROID) || defined(QBDI_PLATFORM_OSX)
#include <unistd.h>
#endif

namespace QBDI {

class LogSys {

private:
    FILE* output;
    std::vector<std::pair<const char*, LogPriority>> filter;

    bool matchFilter(const char* tag, LogPriority priority);

    void writeTag(LogPriority priority, const char* tag);

public:

    LogSys(FILE* output = stderr);

    void setOutput(FILE* output);

    void addFilter(const char *tag, LogPriority priority);

    /*! Output a formatted log entry with the designated priority and tag. The formatted output
     *  follow the fprintf syntax.
     *
     * @param[in] priority   This log entry priority.
     * @param[in] tag        This log entry tag.
     * @param[in] fmt        This log entry format string followed by the formatting argument.
    */
    void log(LogPriority priority, const char* tag, const char* fmt, ...);

    /*! Output a formatted log entry with the designated priority and tag. This version takes a
     * callback in parameter which is called if the tag and priority are matched by a filter and is
     * in charge of printing the log entry. The tag of the log line and the line ending is handled
     * by logsys.
     *
     * @param[in] priority   This log entry priority.
     * @param[in] tag        This log entry tag.
     * @param[in] callback   This log entry callback. It can be a function or a closure.
    */
    void logCallback(LogPriority priority, const char* tag, std::function<void (FILE *log)> callback);

};

/*! Global logging system singleton.
*/
extern QBDI::LogSys LOGSYS;
};

#if defined(_QBDI_LOG_DEBUG)

#define Log(p, t, ...) QBDI::LOGSYS.log(p, t, __VA_ARGS__)
#define LogCallback(p, t, c) QBDI::LOGSYS.logCallback(p, t, c)
#define LogDebug(t, ...) QBDI::LOGSYS.log(QBDI::LogPriority::DEBUG, t, __VA_ARGS__)

#else

#define Log(p, t, ...)

/*! Shortcut macro for ``logCallback``. Disabled in release.
*/
#define LogCallback(p, t, c)

/*! Shortcut macro for ``log(Debug, ...)``. Disabled in release.
*/
#define LogDebug(t, ...)

#endif

/*! Shortcut macro for ``log(Warning, ...)``. Enabled in release.
*/
#define LogWarning(t, ...) QBDI::LOGSYS.log(QBDI::LogPriority::WARNING, t, __VA_ARGS__)

/*! Shortcut macro for ``log(Error, ...)``. Enabled in release.
*/
#define LogError(t, ...) QBDI::LOGSYS.log(QBDI::LogPriority::ERROR, t, __VA_ARGS__)

#define Require(tag, req) if (!(req)) { LogError(tag, "Assertion Failed : %s", #req); }
#define RequireAction(tag, req, ac) if (!(req)) { LogError(tag , "Assertion Failed : %s", #req); ac; }

#endif
