/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2024 Quarkslab
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
#ifndef QBDI_CALLBACK_H_
#define QBDI_CALLBACK_H_

#include "QBDI/Bitmask.h"
#include "QBDI/InstAnalysis.h"
#include "QBDI/Platform.h"
#include "QBDI/State.h"
#ifdef __cplusplus
#include <functional>
#include <vector>

namespace QBDI {
#endif

/*! The callback results.
 */
typedef enum {
  _QBDI_EI(CONTINUE) = 0,    /*!< The execution of the basic block continues. */
  _QBDI_EI(SKIP_INST) = 1,   /*!< Available only with PREINST InstCallback.
                              *   The instruction and the remained PREINST
                              *   callbacks are skip. The execution continue
                              *   with the POSTINST instruction.
                              *
                              *   We recommand to used this result with a low
                              *   priority PREINST callback in order to emulate
                              *   the instruction without skipping the POSTINST
                              *   callback.
                              */
  _QBDI_EI(SKIP_PATCH) = 2,  /*!< Available only with InstCallback. The current
                              *   instruction and the reminding callback (PRE
                              *   and POST) are skip. The execution continues to
                              *   the next instruction.
                              *
                              *   For instruction that change the instruction
                              *   pointer (jump/call/ret), BREAK_TO_VM must be
                              *   used insted of SKIP.
                              *
                              *   SKIP can break the record of MemoryAccess for
                              *   the current instruction.
                              */
  _QBDI_EI(BREAK_TO_VM) = 3, /*!< The execution breaks and returns to the VM
                              *   causing a complete reevaluation of the
                              *   execution state. A BREAK_TO_VM is needed to
                              *   ensure that modifications of the Program
                              *   Counter or the program code are taken
                              *   into account.
                              */
  _QBDI_EI(STOP) = 4,        /*!< Stops the execution of the program. This
                              *   causes the run function to return early.
                              */
} VMAction;

typedef void *VMInstance;

#ifdef __cplusplus
class VM;
using VMInstanceRef = VM *;
#else
typedef VMInstance *VMInstanceRef;
#endif

/*! Instruction callback function type.
 *
 * @param[in] vm            VM instance of the callback.
 * @param[in] gprState      A structure containing the state of the
 *                          General Purpose Registers. Modifying
 *                          it affects the VM execution accordingly.
 * @param[in] fprState      A structure containing the state of the
 *                          Floating Point Registers. Modifying
 *                          it affects the VM execution accordingly.
 * @param[in] data          User defined data which can be defined when
 *                          registering the callback.
 *
 * @return                  The callback result used to signal subsequent
 *                          actions the VM needs to take.
 */
typedef VMAction (*InstCallback)(VMInstanceRef vm, GPRState *gprState,
                                 FPRState *fprState, void *data);
#ifdef __cplusplus
/*! Instruction callback lambda type.
 *
 * @param[in] vm            VM instance of the callback.
 * @param[in] gprState      A structure containing the state of the
 *                          General Purpose Registers. Modifying
 *                          it affects the VM execution accordingly.
 * @param[in] fprState      A structure containing the state of the
 *                          Floating Point Registers. Modifying
 *                          it affects the VM execution accordingly.
 *
 * @return                  The callback result used to signal subsequent
 *                          actions the VM needs to take.
 */
typedef std::function<VMAction(VMInstanceRef vm, GPRState *gprState,
                               FPRState *fprState)>
    InstCbLambda;
#endif

/*! Position relative to an instruction.
 */
typedef enum {
  _QBDI_EI(PREINST) = 0, /*!< Positioned before the instruction.*/
  _QBDI_EI(POSTINST)     /*!< Positioned after the instruction.*/
} InstPosition;

/*! Priority of callback
 *
 * A callback with an higher priority will be call before a callback with a
 * lower priority.
 *
 * ie:
 *
 * 1. CBpre(p = 10)
 * 2. CBpre(p = 0)
 * 3. CBpre(p = -10)
 * 4. instrumented instruction
 * 5. CBpost(p = 10)
 * 6. CBpost(p = 0)
 * 7. CBpost(p = -10)
 *
 * When the MemoryAccess API is used in a callback, the priority of the callback
 * must not be greater than PRIORITY_MEMACCESS_LIMIT
 */
typedef enum {
  _QBDI_EI(PRIORITY_DEFAULT) = 0, /*!< Default priority for callback */
  _QBDI_EI(PRIORITY_MEMACCESS_LIMIT) =
      0x1000000, /*!< Maximum priority if getInstMemoryAccess
                  *   is used in the callback */
} CallbackPriority;

typedef enum {
  _QBDI_EI(NO_EVENT) = 0,
  _QBDI_EI(SEQUENCE_ENTRY) = 1,            /*!< Triggered when the execution
                                            * enters a sequence.
                                            */
  _QBDI_EI(SEQUENCE_EXIT) = 1 << 1,        /*!< Triggered when the execution
                                            * exits from the current sequence.
                                            */
  _QBDI_EI(BASIC_BLOCK_ENTRY) = 1 << 2,    /*!< Triggered when the execution
                                            * enters a basic block.
                                            */
  _QBDI_EI(BASIC_BLOCK_EXIT) = 1 << 3,     /*!< Triggered when the execution
                                            * exits from the current
                                            * basic block.
                                            */
  _QBDI_EI(BASIC_BLOCK_NEW) = 1 << 4,      /*!< Triggered when the execution
                                            * enters a new (~unknown)
                                            * basic block.
                                            */
  _QBDI_EI(EXEC_TRANSFER_CALL) = 1 << 5,   /*!< Triggered when the ExecBroker
                                            * executes an execution transfer.
                                            */
  _QBDI_EI(EXEC_TRANSFER_RETURN) = 1 << 6, /*!< Triggered when the ExecBroker
                                            * returns from an execution
                                            * transfer.
                                            */
  _QBDI_EI(SYSCALL_ENTRY) = 1 << 7,        /*!< Not implemented.*/
  _QBDI_EI(SYSCALL_EXIT) = 1 << 8,         /*!< Not implemented.*/
  _QBDI_EI(SIGNAL) = 1 << 9,               /*!< Not implemented.*/
} VMEvent;

_QBDI_ENABLE_BITMASK_OPERATORS(VMEvent)

/*!
 * Structure describing the current VM state
 */
typedef struct {
  VMEvent event;         /*!< The event(s) which triggered the callback (must
                          * be checked using a mask:
                          * event & BASIC_BLOCK_ENTRY).
                          */
  rword basicBlockStart; /*!< The current basic block start address which can
                          * also be the execution transfer destination.
                          */
  rword basicBlockEnd;   /*!< The current basic block end address which can
                          * also be the execution transfer destination.
                          */
  rword sequenceStart;   /*!< The current sequence start address which can also
                          * be the execution transfer destination.
                          */
  rword sequenceEnd;     /*!< The current sequence end address which can also
                          * be the execution transfer destination.
                          */
  rword lastSignal;      /*!< Not implemented.*/
} VMState;

/*! VM callback function type.
 *
 * @param[in] vm            VM instance of the callback.
 * @param[in] vmState       A structure containing the current state of the VM.
 * @param[in] gprState      A structure containing the state of the
 *                          General Purpose Registers. Modifying
 *                          it affects the VM execution accordingly.
 * @param[in] fprState      A structure containing the state of the
 *                          Floating Point Registers. Modifying
 *                          it affects the VM execution accordingly.
 * @param[in] data          User defined data which can be defined when
 *                          registering the callback.
 *
 * @return                  The callback result used to signal subsequent
 *                          actions the VM needs to take.
 */
typedef VMAction (*VMCallback)(VMInstanceRef vm, const VMState *vmState,
                               GPRState *gprState, FPRState *fprState,
                               void *data);
#ifdef __cplusplus
/*! VM callback lambda type.
 *
 * @param[in] vm            VM instance of the callback.
 * @param[in] vmState       A structure containing the current state of the VM.
 * @param[in] gprState      A structure containing the state of the
 *                          General Purpose Registers. Modifying
 *                          it affects the VM execution accordingly.
 * @param[in] fprState      A structure containing the state of the
 *                          Floating Point Registers. Modifying
 *                          it affects the VM execution accordingly.
 *
 * @return                  The callback result used to signal subsequent
 *                          actions the VM needs to take.
 */
typedef std::function<VMAction(VMInstanceRef vm, const VMState *vmState,
                               GPRState *gprState, FPRState *fprState)>
    VMCbLambda;
#endif

static const uint16_t NO_REGISTRATION = 0xFFFF;
static const uint16_t NOT_FOUND = 0xFFFF;
static const uint16_t ANY = 0xFFFF;

/*! Memory access type (read / write / ...)
 */
typedef enum {
  _QBDI_EI(MEMORY_READ) = 1,       /*!< Memory read access */
  _QBDI_EI(MEMORY_WRITE) = 1 << 1, /*!< Memory write access */
  _QBDI_EI(MEMORY_READ_WRITE) = 3  /*!< Memory read/write access */
} MemoryAccessType;

_QBDI_ENABLE_BITMASK_OPERATORS(MemoryAccessType);

/*! Memory access flags
 */
typedef enum {
  _QBDI_EI(MEMORY_NO_FLAGS) = 0,
  _QBDI_EI(MEMORY_UNKNOWN_SIZE) = 1 << 0,  /*!< The size of the access isn't
                                            * known.
                                            */
  _QBDI_EI(MEMORY_MINIMUM_SIZE) = 1 << 1,  /*!< The given size is a minimum
                                            * size.
                                            */
  _QBDI_EI(MEMORY_UNKNOWN_VALUE) = 1 << 2, /*!< The value of the access is
                                            * unknown or hasn't been retrived.
                                            */
} MemoryAccessFlags;

_QBDI_ENABLE_BITMASK_OPERATORS(MemoryAccessFlags);

/*! Describe a memory access
 */
typedef struct {
  rword instAddress;       /*!< Address of instruction making the access */
  rword accessAddress;     /*!< Address of accessed memory */
  rword value;             /*!< Value read from / written to memory */
  uint16_t size;           /*!< Size of memory access (in bytes) */
  MemoryAccessType type;   /*!< Memory access type (READ / WRITE) */
  MemoryAccessFlags flags; /*!< Memory access flags */
} MemoryAccess;

#ifdef __cplusplus
struct InstrRuleDataCBK {
  InstPosition position; /*!< Relative position of the event callback (PREINST /
                          * POSTINST).
                          */
  InstCallback cbk;      /*!< Address of the function to call when the
                          * instruction is executed.
                          */
  void *data;            /*!< User defined data which will be forward to cbk */

  InstCbLambda
      lambdaCbk; /*!< Lambda callback. Replace cbk and data if not nullptr */

  int priority; /*!< Priority of the callback */

  InstrRuleDataCBK(InstPosition position, InstCallback cbk, void *data,
                   int priority = PRIORITY_DEFAULT)
      : position(position), cbk(cbk), data(data), lambdaCbk(nullptr),
        priority(priority) {}
  InstrRuleDataCBK(InstPosition position, const InstCbLambda &cbk,
                   int priority = PRIORITY_DEFAULT)
      : position(position), cbk(nullptr), data(nullptr), lambdaCbk(cbk),
        priority(priority) {}
  InstrRuleDataCBK(InstPosition position, InstCbLambda &&cbk,
                   int priority = PRIORITY_DEFAULT)
      : position(position), cbk(nullptr), data(nullptr),
        lambdaCbk(std::move(cbk)), priority(priority) {}
};

using InstrRuleDataVec = std::vector<InstrRuleDataCBK> *;
#else
typedef void *InstrRuleDataVec;
#endif

/*! Instrumentation rule callback function type for C API.
 *
 * @param[in] vm     VM instance of the callback.
 * @param[in] inst   AnalysisType of the current instrumented Instruction.
 * @param[in] cbks   An object to add the callback to apply for this
 *                   instruction. InstCallback can be add with
 *                   qbdi_addInstrRuleData.
 * @param[in] data   User defined data which can be defined when registering
 *                   the callback.
 */
typedef void (*InstrRuleCallbackC)(VMInstanceRef vm, const InstAnalysis *inst,
                                   InstrRuleDataVec cbks, void *data);

#ifdef __cplusplus

/*! Instrumentation rule callback function type.
 *
 * @param[in] vm     VM instance of the callback.
 * @param[in] inst   AnalysisType of the current instrumented Instruction.
 * @param[in] data   User defined data which can be defined when registering
 *                   the callback.
 *
 * @return           Return cbk to call when this instruction is run.
 */
typedef std::vector<InstrRuleDataCBK> (*InstrRuleCallback)(
    VMInstanceRef vm, const InstAnalysis *inst, void *data);

/*! Instrumentation rule callback lambda type.
 *
 * @param[in] vm     VM instance of the callback.
 * @param[in] inst   AnalysisType of the current instrumented Instruction.
 *
 * @return           Return cbk to call when this instruction is run.
 */
typedef std::function<std::vector<InstrRuleDataCBK>(VMInstanceRef vm,
                                                    const InstAnalysis *inst)>
    InstrRuleCbLambda;

} // QBDI::
#endif

#endif
