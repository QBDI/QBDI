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
#ifndef VALIDATORENGINE_H
#define VALIDATORENGINE_H

#include <set>
#include <utility>
#include <vector>
#include <map>

#include "Memory.h"
#include "Utility/LogSys.h"

enum LogVerbosity {
    Stat,
    Summary, Detail,
    Full
};

enum ErrorSeverity {
    NoImpact, // No crash, no cascade
    NonCritical, // No crash but cascade
    Critical // crash
};

struct DiffError {
    const char* regName;
    QBDI::rword real;
    QBDI::rword qbdi;
    ErrorSeverity severity;
    uint64_t cascadeID;
    uint64_t causeExecID;
};

struct DiffMap {
    QBDI::rword real;
    QBDI::rword qbdi;
    QBDI::rword lowerOffset;
    QBDI::rword upperOffset;
    uint64_t causeExecID;
};

struct Cascade {
    uint64_t cascadeID;
    QBDI::rword causeAddress;
    ErrorSeverity severity;
    std::vector<uint64_t> execIDs;
    std::vector<uint64_t> similarCascade;
};

class LogEntry {

public:

    uint64_t execID;
    QBDI::rword address;
    char *disassembly;
    QBDI::rword transfer;
    std::vector<ssize_t> errorIDs;

    LogEntry(uint64_t execID, QBDI::rword address, const char* disassembly);

    LogEntry(const LogEntry& copy);

    ~LogEntry();
};

template<typename T1, typename T2> bool KVComp(const std::pair<T1, T2> &a, const std::pair<T1, T2> &b) {
    return a.second > b.second;
}

class ValidatorEngine {
    LogEntry *lastLogEntry;
    LogEntry *curLogEntry;
    std::vector<DiffMap> diffMaps;
    std::vector<LogEntry> savedLogs;
    std::vector<DiffError> errors;
    std::map<std::string, uint64_t> coverage;

    QBDI::rword debugged;
    QBDI::rword instrumented;
    LogVerbosity verbosity;
    uint64_t execID;

    ssize_t logEntryLookup(uint64_t execID);

    void outputLogEntry(const LogEntry& entry);

    std::vector<std::pair<QBDI::rword, QBDI::rword>> getMapsFromPID(pid_t pid);

    template<typename T> ssize_t diff(const char* regName, T real, T qbdi);

    ssize_t diffGPR(unsigned regID, QBDI::rword real, QBDI::rword qbdi);

    template<typename T> ssize_t diffSPR(const char* regName, T real, T qbdi);

public:

    ValidatorEngine(pid_t debugged, pid_t instrumented, LogVerbosity verbosity) :
        lastLogEntry(nullptr), curLogEntry(nullptr), debugged(debugged), 
        instrumented(instrumented), verbosity(verbosity), execID(0) {}

    void signalNewState(QBDI::rword address, const char* mnemonic, const char* disassembly, 
                        const QBDI::GPRState *gprStateDbg, const QBDI::FPRState *fprStateDbg, 
                        const QBDI::GPRState *gprStateInstr, const QBDI::FPRState *fprStateInstr);

    void signalExecTransfer(QBDI::rword address);

    void signalCriticalState();

    void flushLastLog();

    void logCascades();

    void logCoverage(const char* filename);
};

#endif // VALIDATORENGINE_H
