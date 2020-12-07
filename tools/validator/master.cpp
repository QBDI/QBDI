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
#include "master.h"
#include "pipes.h"
#include "validator.h"
#include "validatorengine.h"

#include "Memory.hpp"

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

void start_master(Process* debugged, pid_t instrumented, int ctrlfd, int datafd) {
    const size_t BUFFER_SIZE = 128;
    char mnemonic[BUFFER_SIZE], disassembly[BUFFER_SIZE];
    char* env = nullptr;
    int status;
    FILE *ctrlPipe, *dataPipe;
    QBDI::GPRState gprStateInstr, gprStateDbg;
    QBDI::FPRState fprStateInstr, fprStateDbg;
    QBDI::rword address;
    EVENT event;
    bool running = true;
    int error = 0;

    QBDI::LOGSYS.addFilter("*", QBDI::LogPriority::ERROR);

    // Create communication fifo
    ctrlPipe = fdopen(ctrlfd, "wb");
    dataPipe = fdopen(datafd, "rb");
    if(ctrlPipe == nullptr || dataPipe == nullptr) {
        LogError("Validator::Master", "Could not open communication pipes with instrumented, exiting!");
        exit(VALIDATOR_ERR_PIPE_CREATION_FAIL);
    }

    // Handling verbosity
    LogVerbosity verbosity = LogVerbosity::Stat;
    if((env = getenv("VALIDATOR_VERBOSITY")) != nullptr) {
        if(strcmp(env, "Stat") == 0) verbosity = LogVerbosity::Stat;
        else if(strcmp(env, "Summary") == 0) verbosity = LogVerbosity::Summary;
        else if(strcmp(env, "Detail") == 0) verbosity = LogVerbosity::Detail;
        else if(strcmp(env, "Full") == 0) verbosity = LogVerbosity::Full;
        else LogWarning("Validator::Master", "Did not understood VALIDATOR_VERBOSITY parameter: %s\n", env);
    }

    ValidatorEngine validator(debugged->getPID(), instrumented, verbosity);

    running = true;
    while(running) {
        if(readEvent(&event, dataPipe) != 1) {
            LogError("Validator::Master", "Lost the data pipe, exiting!");
            debugged->continueExecution();
            error = VALIDATOR_ERR_DATA_PIPE_LOST;
            break;
        }
        if(event ==  EVENT::EXIT) {
            debugged->continueExecution();
            break;
        }
        else if(event == EVENT::EXEC_TRANSFER) {
            QBDI::rword transferAddress;
            if(readExecTransferEvent(&transferAddress, dataPipe) != 1) {
                LogError("Validator::Master", "Lost the data pipe, exiting!");
                debugged->continueExecution();
                error = VALIDATOR_ERR_DATA_PIPE_LOST;
                break;
            }
            validator.signalExecTransfer(transferAddress);
        }
        else if(event == EVENT::INSTRUCTION) {
            if(writeCommand(COMMAND::CONTINUE, ctrlPipe) != 1) {
                LogError("Validator::Master", "Lost the control pipe, exiting!");
                debugged->continueExecution();
                error = VALIDATOR_ERR_CTRL_PIPE_LOST;
                break;
            }
            if(readInstructionEvent(&address, mnemonic, BUFFER_SIZE, disassembly, BUFFER_SIZE, &gprStateInstr, &fprStateInstr, dataPipe) != 1) {
                LogError("Validator::Master", "Lost the data pipe, exiting!");
                debugged->continueExecution();
                error = VALIDATOR_ERR_DATA_PIPE_LOST;
                break;
            }
            debugged->setBreakpoint((void*) QBDI_GPR_GET(&gprStateInstr, QBDI::REG_PC));
            do {
                debugged->continueExecution();
                status = debugged->waitForStatus();
                if(hasExited(status)) {
                    LogError("Validator::Master", "Execution diverged, debugged process exited!");
                    validator.signalCriticalState();
                    running = false;
                    writeCommand(COMMAND::STOP, ctrlPipe);
                    error = VALIDATOR_ERR_DBG_EXITED;
                    break;
                }
                else if(hasCrashed(status)) {
                    LogError("Validator::Master", "Something went really wrong, debugged process encoutered signal %d", WSTOPSIG(status));
                    validator.signalCriticalState();
                    running = false;
                    writeCommand(COMMAND::STOP, ctrlPipe);
                    error = VALIDATOR_ERR_DBG_CRASH;
                    break;
                }
                debugged->getProcessGPR(&gprStateDbg);
                debugged->getProcessFPR(&fprStateDbg);
            }
            while(QBDI_GPR_GET(&gprStateDbg, QBDI::REG_PC) != QBDI_GPR_GET(&gprStateInstr, QBDI::REG_PC));
            validator.signalNewState(address, mnemonic, disassembly, &gprStateDbg, &fprStateDbg, &gprStateInstr, &fprStateInstr);
            if (running) {
                debugged->unsetBreakpoint();
            }
        }
    }

    validator.flushLastLog();
    validator.logCascades();
    if((env = getenv("VALIDATOR_COVERAGE")) != nullptr) {
        validator.logCoverage(env);
    }

    exit(error);
}
