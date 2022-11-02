/*
 * This file is part of QBDI.
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
#include "validatorengine.h"
#include <algorithm>
#include <cstring>
#include <inttypes.h>
#include <poll.h>
#include <unistd.h>

std::pair<QBDI::rword, QBDI::rword> getValidOffsetRange(QBDI::rword address,
                                                        pid_t pid) {
  std::vector<QBDI::MemoryMap> maps = QBDI::getRemoteProcessMaps(pid);
  for (QBDI::MemoryMap &m : maps) {
    if (m.range.contains(address)) {
      return std::make_pair(address - m.range.start(),
                            m.range.end() - address - 1);
    }
  }
  return std::make_pair(0, 0);
}

ssize_t ValidatorEngine::logEntryLookup(uint64_t execID) {
  size_t upper = savedLogs.size();
  size_t lower = 0;
  while (upper != lower) {
    size_t i = (upper + lower) / 2;
    if (savedLogs[i]->execID < execID) {
      lower = i;
    } else if (savedLogs[i]->execID > execID) {
      upper = i;
    } else {
      return (ssize_t)i;
    }
  }
  return -1;
}

QBDI::MemoryMap *ValidatorEngine::getModule(QBDI::rword address) {
  static QBDI::MemoryMap *module = nullptr;
  static std::vector<QBDI::MemoryMap> memoryModule;

  // try to find module name of current address
  if (module != nullptr) {
    if (!module->range.contains(address)) {
      module = nullptr;
    }
  }

  if (module == nullptr) {
    for (auto &m : memoryModule) {
      if (m.range.contains(address)) {
        module = &m;
        break;
      }
    }
  }

  if (module == nullptr) {
    memoryModule.clear();
    for (auto &m : QBDI::getRemoteProcessMaps(instrumented)) {
      if (m.permission & QBDI::PF_EXEC) {
        memoryModule.push_back(m);
      }
    }
    for (auto &m : memoryModule) {
      if (m.range.contains(address)) {
        module = &m;
        break;
      }
    }
  }

  return module;
}

void ValidatorEngine::outputLogEntry(const LogEntry &entry,
                                     bool showMemoryError, bool showDiffError) {
  QBDI::MemoryMap *module = getModule(entry.address);

  fprintf(stderr, "ExecID: %" PRIu64 " \t%25s 0x%016" PRIRWORD ": %s\n",
          entry.execID, (module != nullptr) ? module->name.c_str() : "",
          entry.address, entry.disassembly.c_str());
  if (entry.transfer != 0) {
    QBDI::MemoryMap *transfer_module = getModule(entry.transfer);
    fprintf(stderr, "\tCaused a transfer to address 0x%" PRIRWORD " %s\n",
            entry.transfer,
            (transfer_module != nullptr) ? transfer_module->name.c_str() : "");
  }
  if (entry.accessError.get() != nullptr && showMemoryError) {
    const AccessError &access = *entry.accessError;
    fprintf(stderr, "\tMemoryAccess Error (mnemonic : %s):\n",
            entry.mnemonic.c_str());
    if (access.doRead and !access.mayRead) {
      fprintf(stderr, "\t\t- Found unexpected read\n");
    } else if (!access.doRead and access.mayRead) {
      fprintf(stderr, "\t\t- Missing read\n");
    }
    if (access.doWrite and !access.mayWrite) {
      fprintf(stderr, "\t\t- Found unexpected write\n");
    } else if (!access.doWrite and access.mayWrite) {
      fprintf(stderr, "\t\t- Missing write\n");
    }
    for (unsigned i = 0; i < access.accesses.size(); i++) {
      const QBDI::MemoryAccess &a = access.accesses[i];
      fprintf(stderr,
              "\t\t[%d] type=%s%s, addr=0x%" PRIRWORD
              ", size=0x%x, value=0x%" PRIRWORD "\n",
              i, (a.type & QBDI::MemoryAccessType::MEMORY_READ) ? "r" : "",
              (a.type & QBDI::MemoryAccessType::MEMORY_WRITE) ? "w" : "",
              a.accessAddress, a.size, a.value);
    }
  }
  if (showDiffError) {
    for (ssize_t eID : entry.errorIDs) {
      if (errors[eID].severity == ErrorSeverity::NoImpact) {
        fprintf(stderr, "\tError with no impact ");
      } else if (errors[eID].severity == ErrorSeverity::NonCritical) {
        fprintf(stderr, "\tError with non critical impact ");
      } else if (errors[eID].severity == ErrorSeverity::Critical) {
        fprintf(stderr, "\tError with critical impact ");
      }
      fprintf(stderr,
              "on %s: 0x%" PRIRWORD " (real) != 0x%" PRIRWORD " (qbdi)\n",
              errors[eID].regName, errors[eID].real, errors[eID].qbdi);
    }
  }
}

ssize_t ValidatorEngine::diff(const char *regName, QBDI::rword real,
                              QBDI::rword qbdi) {
  // No diff, no error
  if (real == qbdi) {
    return -1;
  }

  DiffError e;
  e.regName = regName;
  e.real = (QBDI::rword)real;
  e.qbdi = (QBDI::rword)qbdi;
  e.severity = ErrorSeverity::NoImpact;
  e.causeExecID = execID;
  e.cascadeID = execID;

  // If lastLogEntry has at least one error, consider this new error to be a
  // cascade
  if (lastLogEntry != nullptr && lastLogEntry->errorIDs.size() > 0) {
    e.cascadeID = errors[lastLogEntry->errorIDs[0]].cascadeID;
    // Upgrade previous error severity to non critical because they caused a
    // cascade
    for (ssize_t eID : lastLogEntry->errorIDs) {
      errors[eID].severity = ErrorSeverity::NonCritical;
    }
  } else {
    // If equivalent error exist, consider this new error to be a cascade
    for (DiffError &prevError : errors) {
      if (e.real == prevError.real && e.qbdi == prevError.qbdi) {
        e.cascadeID = prevError.cascadeID;
        // Upgrade previous error severity to non critical because they caused a
        // cascade
        prevError.severity = ErrorSeverity::NonCritical;
        break;
      }
    }
  }
  // else new cascadeID
  errors.push_back(e);
  return errors.size() - 1;
}

ssize_t ValidatorEngine::diffGPR(unsigned regID, QBDI::rword real,
                                 QBDI::rword qbdi) {
  // No diff, no error
  if (real == qbdi) {
    return -1;
  }

  // First try to explain the diff using the diffmaps
  for (const DiffMap &d : diffMaps) {
    if (real - d.real == qbdi - d.qbdi &&
        ((d.real >= real && d.real - real <= d.lowerOffset) ||
         (real >= d.real && real - d.real <= d.upperOffset))) {
      // Explained, no error
      return -1;
    }
  }

  // If this error is just propagating from the previous state, just copy the
  // pointer
  if (lastLogEntry != nullptr) {
    for (ssize_t eID : lastLogEntry->errorIDs) {
      if (strcmp(QBDI::GPR_NAMES[regID], errors[eID].regName) == 0 &&
          real == errors[eID].real && qbdi == errors[eID].qbdi) {
        return eID;
      }
    }
  }

  // New error at the begining of execution or after an ExecTransfer, add to
  // diffmaps
  if (lastLogEntry == nullptr || curLogEntry->transfer != 0) {
    DiffMap d;
    d.real = real;
    d.qbdi = qbdi;
    std::pair<QBDI::rword, QBDI::rword> rangeDbg =
        getValidOffsetRange(real, debugged);
    std::pair<QBDI::rword, QBDI::rword> rangeInstr =
        getValidOffsetRange(qbdi, instrumented);
    d.lowerOffset = std::min(rangeDbg.first, rangeInstr.first);
    d.upperOffset = std::min(rangeDbg.second, rangeInstr.second);
    d.causeExecID = execID;
    diffMaps.push_back(d);
    return -1;
  }

  // Else normal error diff
  return diff(QBDI::GPR_NAMES[regID], real, qbdi);
}

ssize_t ValidatorEngine::diffSPR(const char *regName, QBDI::rword real,
                                 QBDI::rword qbdi) {
  // No diff, no error
  if (real == qbdi) {
    return -1;
  }

  // First try to explain the diff using the exact diff of the diffmaps
  for (const DiffMap &d : diffMaps) {
    if (d.lowerOffset == 0 && d.upperOffset == 0 && real == d.real &&
        qbdi == d.qbdi) {
      return -1;
    }
  }

  // If this error is just propagating from the previous state, just copy the
  // pointer
  if (lastLogEntry != nullptr) {
    for (ssize_t eID : lastLogEntry->errorIDs) {
      if (strcmp(regName, errors[eID].regName) == 0 &&
          real == errors[eID].real && qbdi == errors[eID].qbdi) {
        return eID;
      }
    }
  }

  // New error at the begining of execution or after an ExecTransfer, add to
  // diffmaps as an exact diff
  if (lastLogEntry == nullptr || curLogEntry->transfer != 0) {
    DiffMap d;
    d.real = real;
    d.qbdi = qbdi;
    d.lowerOffset = 0;
    d.upperOffset = 0;
    d.causeExecID = execID;
    diffMaps.push_back(d);
    return -1;
  }

  // Else normal error diff
  return diff(regName, real, qbdi);
}

void ValidatorEngine::signalNewState(QBDI::rword address, const char *mnemonic,
                                     const char *disassembly, bool skipDebugger,
                                     const QBDI::GPRState *gprStateDbg,
                                     const QBDI::FPRState *fprStateDbg,
                                     const QBDI::GPRState *gprStateInstr,
                                     const QBDI::FPRState *fprStateInstr) {

  if (curLogEntry != nullptr) {
    if (not skipDebugger) {
      compareState(gprStateDbg, fprStateDbg, gprStateInstr, fprStateInstr);
    }
    // If this logEntry generated at least one new error, saved it
    if (!curLogEntry->saved) {
      for (const ssize_t eID : curLogEntry->errorIDs) {
        if (errors[eID].causeExecID == execID) {
          curLogEntry->saved = true;
          break;
        }
      }
      if (curLogEntry->accessError.get() != nullptr) {
        curLogEntry->saved = true;
      }
      if (curLogEntry->saved) {
        savedLogs.push_back(std::unique_ptr<LogEntry>(curLogEntry));
      }
    }
  }

  if (lastLogEntry != nullptr) {
    if (verbosity == LogVerbosity::Full) {
      outputLogEntry(*lastLogEntry);
    }
    if (!lastLogEntry->saved) {
      delete lastLogEntry;
    }
  }
  lastLogEntry = curLogEntry;

  execID++;

  coverage[std::string(mnemonic)]++;
  curLogEntry = new LogEntry(execID, address, disassembly, mnemonic);
}

void ValidatorEngine::signalAccessError(
    QBDI::rword address, bool doRead, bool mayRead, bool doWrite, bool mayWrite,
    const std::vector<QBDI::MemoryAccess> &accesses) {
  if (curLogEntry != nullptr && curLogEntry->accessError.get() == nullptr &&
      curLogEntry->address == address) {
    accessError += 1;
    curLogEntry->accessError = std::make_unique<AccessError>(
        doRead, mayRead, doWrite, mayWrite, accesses);
    memAccessMnemonicSet.insert(curLogEntry->mnemonic);
  }
}

void ValidatorEngine::signalExecTransfer(QBDI::rword address) {
  if (curLogEntry != nullptr) {
    curLogEntry->transfer = address;
  }
}

void ValidatorEngine::signalCriticalState() {
  // Mark current errors as critical
  if (lastLogEntry != nullptr) {
    for (const ssize_t eID : lastLogEntry->errorIDs) {
      errors[eID].severity = ErrorSeverity::Critical;
    }
  }
}

void ValidatorEngine::flushLastLog() {
  if (lastLogEntry != nullptr) {
    if (verbosity == LogVerbosity::Full) {
      outputLogEntry(*lastLogEntry);
    }
    if (!lastLogEntry->saved) {
      delete lastLogEntry;
    }
    lastLogEntry = nullptr;
  }
  if (curLogEntry != nullptr) {
    if (verbosity == LogVerbosity::Full) {
      outputLogEntry(*curLogEntry);
    }
    if (!curLogEntry->saved) {
      delete lastLogEntry;
    }
    curLogEntry = nullptr;
  }
  syncCompareThread();
}

void ValidatorEngine::logCascades() {
  std::vector<Cascade> cascades;
  if (verbosity >= LogVerbosity::Stat) {
    size_t noImpactCount = 0;
    size_t nonCriticalCount = 0;
    size_t criticalCount = 0;
    fprintf(stderr, "Stats\n");
    fprintf(stderr, "=====\n\n");
    fprintf(stderr, "Executed %" PRIu64 " total instructions\n", execID);
    fprintf(stderr, "Executed %zu unique instructions\n", coverage.size());
    fprintf(stderr, "Encountered %zu difference mappings\n", diffMaps.size());
    fprintf(stderr, "Encountered %" PRIu64 " memoryAccess errors\n",
            accessError);
    fprintf(stderr, "Encountered %zu memoryAccess unique errors\n",
            memAccessMnemonicSet.size());
    fprintf(stderr, "SizeOutput: %zu %zu\n", outputDbg.size(),
                                               outputDbi.size());
    if (outputDbg.size() == outputDbi.size() and
        memcmp(outputDbg.data(), outputDbi.data(), outputDbi.size()) == 0) {
      fprintf(stderr, "SameOutput: True\n");
    } else {
      fprintf(stderr, "SameOutput: False\n");
    }
    fprintf(stderr, "Encountered %zu errors:\n", errors.size());
    // Compute error stats, assemble cascades
    for (const DiffError &error : errors) {
      bool found = false;
      for (size_t i = 0; i < cascades.size(); i++) {
        if (cascades[i].cascadeID == error.cascadeID) {
          found = true;
          cascades[i].execIDs.push_back(error.causeExecID);
          if (error.severity > cascades[i].severity) {
            cascades[i].severity = error.severity;
          }
        }
      }
      if (!found) {
        cascades.push_back(Cascade{
            error.cascadeID,
            savedLogs[logEntryLookup(error.causeExecID)]->address,
            error.severity, std::vector<uint64_t>(), std::vector<uint64_t>()});
        cascades.back().execIDs.push_back(error.causeExecID);
      }

      if (error.severity == ErrorSeverity::NoImpact)
        noImpactCount++;
      else if (error.severity == ErrorSeverity::NonCritical)
        nonCriticalCount++;
      else if (error.severity == ErrorSeverity::Critical)
        criticalCount++;
    }
    fprintf(stderr, "\tNo impact errors: %zu\n", noImpactCount);
    fprintf(stderr, "\tNon critical errors: %zu\n", nonCriticalCount);
    fprintf(stderr, "\tCritical errors: %zu\n", criticalCount);
    fprintf(stderr, "Encountered %zu error cascades:\n", cascades.size());
    // Compute cascade stats
    noImpactCount = 0;
    nonCriticalCount = 0;
    criticalCount = 0;
    for (size_t i = 0; i < cascades.size(); i++) {
      if (cascades[i].severity == ErrorSeverity::NoImpact)
        noImpactCount++;
      else if (cascades[i].severity == ErrorSeverity::NonCritical)
        nonCriticalCount++;
      else if (cascades[i].severity == ErrorSeverity::Critical)
        criticalCount++;
    }
    fprintf(stderr, "\tNo impact cascades: %zu\n", noImpactCount);
    fprintf(stderr, "\tNon critical cascades: %zu\n", nonCriticalCount);
    fprintf(stderr, "\tCritical cascades: %zu\n", criticalCount);
  }
  if (verbosity >= LogVerbosity::Summary) {
    fprintf(stderr, "\n\n");
    fprintf(stderr, "Error MemoryAccess:\n");
    fprintf(stderr, "==============\n\n");
    for (auto &l : savedLogs) {
      if (l->accessError.get() != nullptr) {
        outputLogEntry(*l, true, false);
      }
    }
    fprintf(stderr, "\n\n");
    fprintf(stderr, "Error cascades:\n");
    fprintf(stderr, "==============\n\n");
    // Simplify, remove duplicates and sort cascades
    for (size_t i = 0; i < cascades.size();) {
      size_t eidx = 0;

      while (eidx < cascades[i].execIDs.size() - 1) {
        if (cascades[i].execIDs[eidx] == cascades[i].execIDs[eidx + 1]) {
          cascades[i].execIDs.erase(cascades[i].execIDs.begin() + eidx + 1);
        } else {
          eidx++;
        }
      }

      bool sameCascade = false;
      for (size_t j = 0; j < i; j++) {
        // Check for duplicate
        if (cascades[j].causeAddress == cascades[i].causeAddress &&
            cascades[j].severity == cascades[i].severity) {

          /* More strict similarity test, disabled because too strict
         cascades[j].execIDs.size() == cascades[i].execIDs.size()) {

          for(size_t k = 0; k < cascades[i].execIDs.size(); k++) {
              if(logEntryLookup(cascades[j].execIDs[k])->address !=
                 logEntryLookup(cascades[i].execIDs[k])->address) {
                  sameCascade = false;
                  break;
              }
          }*/
          sameCascade = true;
        }

        // Remove if duplicate
        if (sameCascade) {
          cascades[j].similarCascade.push_back(cascades[i].cascadeID);
          cascades[i] = cascades.back();
          cascades.pop_back();
          break;
        }
        // Sort
        else {
          if (cascades[i].severity > cascades[j].severity) {
            Cascade tmp = cascades[j];
            cascades[j] = cascades[i];
            cascades[i] = tmp;
          }
        }
      }
      // Conditionnal increment if we didn't suppress cascade i as duplicate
      if (sameCascade == false) {
        i++;
      }
    }
    for (size_t i = 0; i < cascades.size(); i++) {
      fprintf(stderr, "Cascade %" PRIu64 ":\n", cascades[i].cascadeID);
      fprintf(stderr, "--------------------\n\n");
      fprintf(stderr, "%zu other similar cascade encountered\n",
              cascades[i].similarCascade.size());
      fprintf(stderr, "Cascade length: %zu\n", cascades[i].execIDs.size());

      if (cascades[i].severity == ErrorSeverity::NoImpact)
        fprintf(stderr, "No Impact classification\n");
      else if (cascades[i].severity == ErrorSeverity::NonCritical)
        fprintf(stderr, "Non Critical Impact classification\n");
      else if (cascades[i].severity == ErrorSeverity::Critical)
        fprintf(stderr, "Critical Impact classification\n");

      if (verbosity == LogVerbosity::Summary) {
        fprintf(stderr, "Cause:\n");
        outputLogEntry(*savedLogs[logEntryLookup(cascades[i].cascadeID)].get(),
                       false);
      } else if (verbosity >= LogVerbosity::Detail) {
        fprintf(stderr, "Chain:\n");
        for (ssize_t eID : cascades[i].execIDs) {
          outputLogEntry(*savedLogs[logEntryLookup(eID)].get(), false);
        }
      }
      fprintf(stderr, "\n\n");
    }
  }
}

void ValidatorEngine::logCoverage(const char *filename) {
  FILE *coverageFile = fopen(filename, "w");
  std::vector<std::pair<std::string, uint64_t>> coverageList;

  for (const auto &c : coverage) {
    coverageList.push_back(std::make_pair(c.first, c.second));
  }
  std::sort(coverageList.begin(), coverageList.end(),
            KVComp<std::string, uint64_t>);
  for (const std::pair<std::string, uint64_t> &c : coverageList) {
    fprintf(coverageFile, "%s: %" PRIu64 "\n", c.first.c_str(), c.second);
  }

  fclose(coverageFile);
}


void ValidatorEngine::startCompareThread() {
  threadStop = false;
  //outputCMPThread = std::thread(&ValidatorEngine::outputCompareThread, this);
}

void ValidatorEngine::outputCompareThread() {
  uint8_t buffer[4096];
  outputDbg.clear();
  outputDbi.clear();

  int retryBeforeExit = 2;

  while (retryBeforeExit > 0) {
    struct pollfd fds[2];

    fds[0].fd = stdoutDbg;
    fds[1].fd = stdoutDbi;
    fds[0].events = POLLIN | POLLRDBAND;
    fds[1].events = POLLIN | POLLRDBAND;

    int ret = poll(fds, 2, 100);

    if (ret < 0) {
      perror("select()");
      break;
    }

    bool hasRead = false;

    if (fds[0].revents & (POLLIN | POLLRDBAND)) {
      size_t readLen = read(stdoutDbg, buffer, sizeof(buffer));
      outputDbg.insert(outputDbg.end(), buffer, buffer + readLen);
      hasRead = true;
    }

    if (fds[1].revents & (POLLIN | POLLRDBAND)) {
      size_t readLen = read(stdoutDbi, buffer, sizeof(buffer));
      outputDbi.insert(outputDbi.end(), buffer, buffer + readLen);
      hasRead = true;
    }

    if (threadStop and not hasRead) {
      retryBeforeExit -= 1;
    }
  }
}

void ValidatorEngine::syncCompareThread() {
  threadStop = true;
  //outputCMPThread.join();
  outputCompareThread();
}
