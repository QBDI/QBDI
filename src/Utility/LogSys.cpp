/*
 * This file is part of QBDI.
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
#include "Utility/LogSys.h"

namespace QBDI {

LogSys LOGSYS;

LogSys::LogSys(FILE* output) : output(output) {
}

void LogSys::setOutput(FILE* output) {
    this->output = output;
}

void LogSys::addFilter(const char *tag, LogPriority priority) {
    filter.push_back(std::make_pair(tag, priority));
}

bool LogSys::matchFilter(const char* tag, LogPriority priority) {
    for(const std::pair<const char*, LogPriority> &f : filter) {
        if(priority >= f.second) {
            bool matches = true;
            int i = 0, j = 0, b = -1;

            // Simple fuzzy match with '*' handling
            while(f.first[i] != '\0') {
                if(f.first[i] == '*') {
                    if(f.first[i+1] == tag[j+1]) {
                        // Mark the '*' as a backtrack point
                        b = i;
                        i++;
                    }
                    j++;
                }
                else if(f.first[i] == tag[j]) {
                    i++;
                    j++;
                }
                else {
                    // No preceding '*' we can backtrack to
                    if(b != -1) {
                        i = b;
                    }
                    else {
                        matches = false;
                        break;
                    }
                }
            }

            if(matches) {
                return true;
            }
        }
    }

    return false;
}

void LogSys::writeTag(LogPriority priority, const char* tag) {
#if defined(QBDI_OS_LINUX) || defined(QBDI_OS_ANDROID) || defined(QBDI_OS_DARWIN)
    if(isatty(fileno(output))) {
        switch(priority) {
            case LogPriority::DEBUG:
                fprintf(output, "\x1b[32;1m[%s]\x1b[0m ", tag);
                break;
            case LogPriority::WARNING:
                fprintf(output, "\x1b[33;1m[%s]\x1b[0m ", tag);
                break;
            case LogPriority::ERROR:
                fprintf(output, "\x1b[31;1m[%s]\x1b[0m ", tag);
                break;
        }
        return;
    }
    #endif
    fprintf(output, "[%s] ", tag);
}

void LogSys::log(LogPriority priority, const char* tag, const char* fmt, ...) {
    if(matchFilter(tag, priority)) {
        va_list ap;

        writeTag(priority, tag);
        va_start(ap, fmt);
        vfprintf(output, fmt, ap);
        va_end(ap);
        fprintf(output, "\n");
    }
}

void LogSys::logCallback(LogPriority priority, const char* tag, std::function<void (FILE *log)> callback) {
    if(matchFilter(tag, priority)) {
        writeTag(priority, tag);
        callback(output);
        fprintf(output, "\n");
    }
}

// C APIs
void qbdi_setLogOutput(FILE* output) {
    if (output == nullptr) {
        output = stderr;
    }
    LOGSYS.setOutput(output);
}

void qbdi_addLogFilter(const char *tag, LogPriority priority) {
    if (tag == nullptr) {
        tag = "*";
    }
    LOGSYS.addFilter(tag, priority);
}

}
