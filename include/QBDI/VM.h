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
#ifndef QBDI_VM_H_
#define QBDI_VM_H_

#include <cstdarg>
#include <forward_list>
#include <memory>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

#include "QBDI/Bitmask.h"
#include "QBDI/Callback.h"
#include "QBDI/Errors.h"
#include "QBDI/InstAnalysis.h"
#include "QBDI/Options.h"
#include "QBDI/Platform.h"
#include "QBDI/Range.h"
#include "QBDI/State.h"

namespace QBDI {

// Forward declaration of engine class
class Engine;
// Forward declaration of private memCBInfo
struct MemCBInfo;
// Forward declaration of private InstrCBInfo
struct InstrCBInfo;

class VM {
private:
  // Private internal engine
  std::unique_ptr<Engine> engine;
  uint8_t memoryLoggingLevel;
  std::unique_ptr<std::vector<std::pair<uint32_t, MemCBInfo>>> memCBInfos;
  uint32_t memCBID;
  uint32_t memReadGateCBID;
  uint32_t memWriteGateCBID;
  std::unique_ptr<
      std::vector<std::pair<uint32_t, std::unique_ptr<InstrCBInfo>>>>
      instrCBInfos;
  std::forward_list<std::pair<uint32_t, VMCbLambda>> vmCBData;
  std::forward_list<std::pair<uint32_t, InstCbLambda>> instCBData;
  std::forward_list<std::pair<uint32_t, InstrRuleCbLambda>> instrRuleCBData;

  uint32_t backupErrno;

public:
  /*! Construct a new VM for a given CPU with specific attributes
   *
   * @param[in] cpu    The name of the CPU
   * @param[in] mattrs A list of additional attributes
   * @param[in] opts   The options to enable in the VM
   */
  QBDI_EXPORT VM(const std::string &cpu = "",
                 const std::vector<std::string> &mattrs = {},
                 Options opts = Options::NO_OPT);

  QBDI_EXPORT ~VM();

  /*! Move constructors.
   *  All the cache is keep.
   *  All registered callbacks will be called with the new pointer of the VM.
   *
   * @param[in] vm     The VM to move
   */
  QBDI_EXPORT VM(VM &&vm);

  /*! Move assignment operator
   *  All the cache is keep.
   *  All registered callbacks will be called with the new pointer of the VM.
   *  This operator mustn't be called when the target VM runs.
   *
   * @param[in] vm     The VM to move
   */
  QBDI_EXPORT VM &operator=(VM &&vm);

  /*! Copy constructors
   *  The state and the configuration is copied. The cache isn't duplicate.
   *  The assigned VM begin with an empty cache.
   *
   * @param[in] vm     The VM to copy
   */
  QBDI_EXPORT VM(const VM &vm);

  /*! Copy assignment operator
   *  The state and the configuration is copied. The cache isn't duplicate.
   *  The assigned VM begin with an empty cache.
   *  This operator mustn't be called when the target VM runs.
   *
   * @param[in] vm     The VM to copy
   */
  QBDI_EXPORT VM &operator=(const VM &vm);

  // delete const move constructor and const move assignment operator
  QBDI_EXPORT VM(const VM &&vm) = delete;
  QBDI_EXPORT VM &operator=(const VM &&vm) = delete;

  /*! Obtain the current general purpose register state.
   *
   * @return A structure containing the GPR state.
   */
  QBDI_EXPORT GPRState *getGPRState() const;

  /*! Obtain the current floating point register state.
   *
   * @return A structure containing the FPR state.
   */
  QBDI_EXPORT FPRState *getFPRState() const;

  /*! Obtain the backuped value of errno, if the option
   * OPT_DISABLE_ERRNO_BACKUP is not enable.
   *
   * @return the backupped value of errno
   */
  QBDI_EXPORT uint32_t getErrno() { return backupErrno; }

  /*! Set the GPR state
   *
   * @param[in] gprState A structure containing the GPR state.
   */
  QBDI_EXPORT void setGPRState(const GPRState *gprState);

  /*! Set the FPR state
   *
   * @param[in] fprState A structure containing the FPR state.
   */
  QBDI_EXPORT void setFPRState(const FPRState *fprState);

  /*! Set the backuped value of errno, if the option
   * OPT_DISABLE_ERRNO_BACKUP is not enable.
   *
   * @param[in] backupErrno_ the value to set
   */
  QBDI_EXPORT void setErrno(uint32_t backupErrno_) {
    backupErrno = backupErrno_;
  }

  /*! Get the current Options of the VM
   */
  QBDI_EXPORT Options getOptions() const;

  /*! Set the Options of the VM
   *  This method mustn't be called when the VM runs.
   *
   * @param[in] options  the new options of the VM
   *
   * If the new options is different that the current ones, the cache will be
   * clear.
   */
  QBDI_EXPORT void setOptions(Options options);

  /*! Add an address range to the set of instrumented address ranges.
   *
   * @param[in] start  Start address of the range (included).
   * @param[in] end    End address of the range (excluded).
   */
  QBDI_EXPORT void addInstrumentedRange(rword start, rword end);

  /*! Add the executable address ranges of a module to the set of instrumented
   * address ranges.
   *
   * @param[in] name  The module's name.
   *
   * @return  True if at least one range was added to the instrumented ranges.
   */
  QBDI_EXPORT bool addInstrumentedModule(const std::string &name);

  /*! Add the executable address ranges of a module to the set of instrumented
   * address ranges using an address belonging to the module.
   *
   * @param[in] addr  An address contained by module's range.
   *
   * @return  True if at least one range was added to the instrumented ranges.
   */
  QBDI_EXPORT bool addInstrumentedModuleFromAddr(rword addr);

  /*! Adds all the executable memory maps to the instrumented range set.
   * @return  True if at least one range was added to the instrumented ranges.
   */
  QBDI_EXPORT bool instrumentAllExecutableMaps();

  /*! Remove an address range from the set of instrumented address ranges.
   *
   * @param[in] start  Start address of the range (included).
   * @param[in] end    End address of the range (excluded).
   */
  QBDI_EXPORT void removeInstrumentedRange(rword start, rword end);

  /*! Remove the executable address ranges of a module from the set of
   * instrumented address ranges.
   *
   * @param[in] name  The module's name.
   *
   * @return  True if at least one range was removed from the instrumented
   * ranges.
   */
  QBDI_EXPORT bool removeInstrumentedModule(const std::string &name);

  /*! Remove the executable address ranges of a module from the set of
   * instrumented address ranges using an address belonging to the module.
   *
   * @param[in] addr  An address contained by module's range.
   *
   * @return  True if at least one range was removed from the instrumented
   * ranges.
   */
  QBDI_EXPORT bool removeInstrumentedModuleFromAddr(rword addr);

  /*! Remove all instrumented ranges.
   */
  QBDI_EXPORT void removeAllInstrumentedRanges();

  /*! Start the execution by the DBI.
   *  This method mustn't be called if the VM already runs.
   *
   * @param[in] start  Address of the first instruction to execute.
   * @param[in] stop   Stop the execution when this instruction is reached.
   *
   * @return  True if at least one block has been executed.
   */
  QBDI_EXPORT bool run(rword start, rword stop);

  /*! Call a function using the DBI (and its current state).
   *  This method mustn't be called if the VM already runs.
   *
   * @param[in] [retval]   Pointer to the returned value (optional).
   * @param[in] function   Address of the function start instruction.
   * @param[in] args       A list of arguments.
   *
   * @return  True if at least one block has been executed.
   *
   * @details Example:
   *
   *     // perform (with QBDI) a call similar to (*funcPtr)(42);
   *     uint8_t *fakestack = nullptr;
   *     QBDI::VM *vm = new QBDI::VM();
   *     QBDI::GPRState *state = vm->getGPRState();
   *     QBDI::allocateVirtualStack(state, 0x1000000, &fakestack);
   *     vm->addInstrumentedModuleFromAddr(funcPtr);
   *     rword retVal;
   *     vm->call(&retVal, funcPtr, {42});
   *     QBDI::alignedFree(fakestack);
   *
   */
  QBDI_EXPORT bool call(rword *retval, rword function,
                        const std::vector<rword> &args = {});

  /*! Call a function using the DBI (and its current state).
   *  This method mustn't be called if the VM already runs.
   *
   * @param[in] [retval]   Pointer to the returned value (optional).
   * @param[in] function   Address of the function start instruction.
   * @param[in] argNum     The number of arguments in the array of arguments.
   * @param[in] args       An array of arguments.
   *
   * @return  True if at least one block has been executed.
   */
  QBDI_EXPORT bool callA(rword *retval, rword function, uint32_t argNum,
                         const rword *args);

  /*! Call a function using the DBI (and its current state).
   *  This method mustn't be called if the VM already runs.
   *
   * @param[in] [retval]   Pointer to the returned value (optional).
   * @param[in] function   Address of the function start instruction.
   * @param[in] argNum     The number of arguments in the variadic list.
   * @param[in] ap         An stdarg va_list object.
   *
   * @return  True if at least one block has been executed.
   */
  QBDI_EXPORT bool callV(rword *retval, rword function, uint32_t argNum,
                         va_list ap);

  /*! Switch the stack and call a function using the DBI (and its current
   *  state).
   *  This method will allocate a new stack and switch to this stack. The
   *  remaining space on the current stack will be use by the called method.
   *  This method mustn't be called if the VM already runs.
   *  The stack pointer in the state must'nt be used after the end of this
   *  method.
   *
   * @param[in] [retval]   Pointer to the returned value (optional).
   * @param[in] function   Address of the function start instruction.
   * @param[in] args       A list of arguments.
   * @param[in] stackSize  The size of the stack for the engine.
   *
   * @return  True if at least one block has been executed.
   *
   * @details Example:
   *
   *     // perform (with QBDI) a call similar to (*funcPtr)(42);
   *     QBDI::VM *vm = new QBDI::VM();
   *     QBDI::GPRState *state = vm->getGPRState();
   *     vm->addInstrumentedModuleFromAddr(funcPtr);
   *     rword retVal;
   *     vm->switchStackAndCall(&retVal, funcPtr, {42});
   *
   */

  QBDI_EXPORT bool switchStackAndCall(rword *retval, rword function,
                                      const std::vector<rword> &args = {},
                                      uint32_t stackSize = 0x20000);

  /*! Switch the stack and call a function using the DBI (and its current
   *  state).
   *  This method mustn't be called if the VM already runs.
   *  The stack pointer in the state must'nt be used after the end of this
   *  method.
   *
   * @param[in] [retval]   Pointer to the returned value (optional).
   * @param[in] function   Address of the function start instruction.
   * @param[in] argNum     The number of arguments in the array of arguments.
   * @param[in] args       An array of arguments.
   * @param[in] stackSize  The size of the stack for the engine.
   *
   * @return  True if at least one block has been executed.
   */
  QBDI_EXPORT bool switchStackAndCallA(rword *retval, rword function,
                                       uint32_t argNum, const rword *args,
                                       uint32_t stackSize = 0x20000);

  /*! Switch the stack and call a function using the DBI (and its current
   *  state).
   *  This method mustn't be called if the VM already runs.
   *  The stack pointer in the state must'nt be used after the end of this
   *  method.
   *
   * @param[in] [retval]   Pointer to the returned value (optional).
   * @param[in] function   Address of the function start instruction.
   * @param[in] argNum     The number of arguments in the variadic list.
   * @param[in] ap         An stdarg va_list object.
   * @param[in] stackSize  The size of the stack for the engine.
   *
   * @return  True if at least one block has been executed.
   */
  QBDI_EXPORT bool switchStackAndCallV(rword *retval, rword function,
                                       uint32_t argNum, va_list ap,
                                       uint32_t stackSize = 0x20000);

  /*! Add a custom instrumentation rule to the VM.
   *
   * @param[in] cbk       A function pointer to the callback
   * @param[in] type      Analyse type needed for this instruction function
   *                      pointer to the callback
   * @param[in] data      User defined data passed to the callback.
   *
   * @return The id of the registered instrumentation
   * (or VMError::INVALID_EVENTID in case of failure).
   */
  QBDI_EXPORT uint32_t addInstrRule(InstrRuleCallback cbk, AnalysisType type,
                                    void *data);

  // register C like InstrRuleCallback
  QBDI_EXPORT uint32_t addInstrRule(InstrRuleCallbackC cbk, AnalysisType type,
                                    void *data);

  /*! Add a custom instrumentation rule to the VM.
   *
   * @param[in] cbk       A lambda function to the callback
   * @param[in] type      Analyse type needed for this instruction function
   *                      pointer to the callback
   *
   * @return The id of the registered instrumentation
   * (or VMError::INVALID_EVENTID in case of failure).
   */
  QBDI_EXPORT uint32_t addInstrRule(const InstrRuleCbLambda &cbk,
                                    AnalysisType type);
  QBDI_EXPORT uint32_t addInstrRule(InstrRuleCbLambda &&cbk, AnalysisType type);

  /*! Add a custom instrumentation rule to the VM on a specify range
   *
   * @param[in] start     Begin of the range of address where apply the rule
   * @param[in] end       End of the range of address where apply the rule
   * @param[in] cbk       A function pointer to the callback
   * @param[in] type      Analyse type needed for this instruction function
   *                      pointer to the callback
   * @param[in] data      User defined data passed to the callback.
   *
   * @return The id of the registered instrumentation (or
   * VMError::INVALID_EVENTID in case of failure).
   */
  QBDI_EXPORT uint32_t addInstrRuleRange(rword start, rword end,
                                         InstrRuleCallback cbk,
                                         AnalysisType type, void *data);

  // register C like InstrRuleCallback
  QBDI_EXPORT uint32_t addInstrRuleRange(rword start, rword end,
                                         InstrRuleCallbackC cbk,
                                         AnalysisType type, void *data);

  /*! Add a custom instrumentation rule to the VM on a specify range
   *
   * @param[in] start     Begin of the range of address where apply the rule
   * @param[in] end       End of the range of address where apply the rule
   * @param[in] cbk       A lambda function to the callback
   * @param[in] type      Analyse type needed for this instruction function
   *                      pointer to the callback
   *
   * @return The id of the registered instrumentation (or
   * VMError::INVALID_EVENTID in case of failure).
   */
  QBDI_EXPORT uint32_t addInstrRuleRange(rword start, rword end,
                                         const InstrRuleCbLambda &cbk,
                                         AnalysisType type);
  QBDI_EXPORT uint32_t addInstrRuleRange(rword start, rword end,
                                         InstrRuleCbLambda &&cbk,
                                         AnalysisType type);

  /*! Add a custom instrumentation rule to the VM on a specify set of range
   *
   * @param[in] range     Range of address where apply the rule
   * @param[in] cbk       A function pointer to the callback
   * @param[in] type      Analyse type needed for this instruction function
   *                      pointer to the callback
   * @param[in] data      User defined data passed to the callback.
   *
   * @return The id of the registered instrumentation
   * (or VMError::INVALID_EVENTID in case of failure).
   */
  QBDI_EXPORT uint32_t addInstrRuleRangeSet(RangeSet<rword> range,
                                            InstrRuleCallback cbk,
                                            AnalysisType type, void *data);

  /*! Add a custom instrumentation rule to the VM on a specify set of range
   *
   * @param[in] range     Range of address where apply the rule
   * @param[in] cbk       A lambda function to the callback
   * @param[in] type      Analyse type needed for this instruction function
   *                      pointer to the callback
   *
   * @return The id of the registered instrumentation
   * (or VMError::INVALID_EVENTID in case of failure).
   */
  QBDI_EXPORT uint32_t addInstrRuleRangeSet(RangeSet<rword> range,
                                            const InstrRuleCbLambda &cbk,
                                            AnalysisType type);
  QBDI_EXPORT uint32_t addInstrRuleRangeSet(RangeSet<rword> range,
                                            InstrRuleCbLambda &&cbk,
                                            AnalysisType type);

  /*! Register a callback event if the instruction matches the mnemonic.
   *
   * @param[in] mnemonic   Mnemonic to match.
   * @param[in] pos        Relative position of the event callback
   *                       (PREINST / POSTINST).
   * @param[in] cbk        A function pointer to the callback.
   * @param[in] data       User defined data passed to the callback.
   * @param[in] priority   The priority of the callback.
   *
   * @return The id of the registered instrumentation (or
   * VMError::INVALID_EVENTID in case of failure).
   */
  QBDI_EXPORT uint32_t addMnemonicCB(const char *mnemonic, InstPosition pos,
                                     InstCallback cbk, void *data,
                                     int priority = PRIORITY_DEFAULT);

  /*! Register a callback event if the instruction matches the mnemonic.
   *
   * @param[in] mnemonic   Mnemonic to match.
   * @param[in] pos        Relative position of the event callback
   *                       (PREINST / POSTINST).
   * @param[in] cbk        A lambda function to the callback
   * @param[in] priority   The priority of the callback.
   *
   * @return The id of the registered instrumentation (or
   * VMError::INVALID_EVENTID in case of failure).
   */
  QBDI_EXPORT uint32_t addMnemonicCB(const char *mnemonic, InstPosition pos,
                                     const InstCbLambda &cbk,
                                     int priority = PRIORITY_DEFAULT);
  QBDI_EXPORT uint32_t addMnemonicCB(const char *mnemonic, InstPosition pos,
                                     InstCbLambda &&cbk,
                                     int priority = PRIORITY_DEFAULT);

  /*! Register a callback event for every instruction executed.
   *
   * @param[in] pos        Relative position of the event callback
   *                       (PREINST / POSTINST).
   * @param[in] cbk        A function pointer to the callback.
   * @param[in] data       User defined data passed to the callback.
   * @param[in] priority   The priority of the callback.
   *
   * @return The id of the registered instrumentation
   * (or VMError::INVALID_EVENTID in case of failure).
   */
  QBDI_EXPORT uint32_t addCodeCB(InstPosition pos, InstCallback cbk, void *data,
                                 int priority = PRIORITY_DEFAULT);

  /*! Register a callback event for every instruction executed.
   *
   * @param[in] pos        Relative position of the event callback
   *                       (PREINST / POSTINST).
   * @param[in] cbk        A lambda function to the callback
   * @param[in] priority   The priority of the callback.
   *
   * @return The id of the registered instrumentation
   * (or VMError::INVALID_EVENTID in case of failure).
   */
  QBDI_EXPORT uint32_t addCodeCB(InstPosition pos, const InstCbLambda &cbk,
                                 int priority = PRIORITY_DEFAULT);
  QBDI_EXPORT uint32_t addCodeCB(InstPosition pos, InstCbLambda &&cbk,
                                 int priority = PRIORITY_DEFAULT);

  /*! Register a callback for when a specific address is executed.
   *
   * @param[in] address  Code address which will trigger the callback.
   * @param[in] pos      Relative position of the callback (PREINST / POSTINST).
   * @param[in] cbk      A function pointer to the callback.
   * @param[in] data     User defined data passed to the callback.
   * @param[in] priority The priority of the callback.
   *
   * @return The id of the registered instrumentation (or
   * VMError::INVALID_EVENTID in case of failure).
   */
  QBDI_EXPORT uint32_t addCodeAddrCB(rword address, InstPosition pos,
                                     InstCallback cbk, void *data,
                                     int priority = PRIORITY_DEFAULT);

  /*! Register a callback for when a specific address is executed.
   *
   * @param[in] address  Code address which will trigger the callback.
   * @param[in] pos      Relative position of the callback (PREINST / POSTINST).
   * @param[in] cbk      A lambda function to the callback
   * @param[in] priority The priority of the callback.
   *
   * @return The id of the registered instrumentation (or
   * VMError::INVALID_EVENTID in case of failure).
   */
  QBDI_EXPORT uint32_t addCodeAddrCB(rword address, InstPosition pos,
                                     const InstCbLambda &cbk,
                                     int priority = PRIORITY_DEFAULT);
  QBDI_EXPORT uint32_t addCodeAddrCB(rword address, InstPosition pos,
                                     InstCbLambda &&cbk,
                                     int priority = PRIORITY_DEFAULT);

  /*! Register a callback for when a specific address range is executed.
   *
   * @param[in] start    Start of the address range which will trigger
   *                     the callback.
   * @param[in] end      End of the address range which will trigger
   *                     the callback.
   * @param[in] pos      Relative position of the callback (PREINST / POSTINST).
   * @param[in] cbk      A function pointer to the callback.
   * @param[in] data     User defined data passed to the callback.
   * @param[in] priority The priority of the callback.
   *
   * @return The id of the registered instrumentation (or
   * VMError::INVALID_EVENTID in case of failure).
   */
  QBDI_EXPORT uint32_t addCodeRangeCB(rword start, rword end, InstPosition pos,
                                      InstCallback cbk, void *data,
                                      int priority = PRIORITY_DEFAULT);

  /*! Register a callback for when a specific address range is executed.
   *
   * @param[in] start    Start of the address range which will trigger
   *                     the callback.
   * @param[in] end      End of the address range which will trigger
   *                     the callback.
   * @param[in] pos      Relative position of the callback (PREINST / POSTINST).
   * @param[in] cbk      A lambda function to the callback
   * @param[in] priority The priority of the callback.
   *
   * @return The id of the registered instrumentation (or
   * VMError::INVALID_EVENTID in case of failure).
   */
  QBDI_EXPORT uint32_t addCodeRangeCB(rword start, rword end, InstPosition pos,
                                      const InstCbLambda &cbk,
                                      int priority = PRIORITY_DEFAULT);
  QBDI_EXPORT uint32_t addCodeRangeCB(rword start, rword end, InstPosition pos,
                                      InstCbLambda &&cbk,
                                      int priority = PRIORITY_DEFAULT);

  /*! Register a callback event for every memory access matching the type
   * bitfield made by the instructions.
   *
   * @param[in] type       A mode bitfield: either QBDI::MEMORY_READ,
   *                       QBDI::MEMORY_WRITE or both (QBDI::MEMORY_READ_WRITE).
   * @param[in] cbk        A function pointer to the callback.
   * @param[in] data       User defined data passed to the callback.
   * @param[in] priority   The priority of the callback.
   *
   * @return The id of the registered instrumentation
   * (or VMError::INVALID_EVENTID in case of failure).
   */
  QBDI_EXPORT uint32_t addMemAccessCB(MemoryAccessType type, InstCallback cbk,
                                      void *data,
                                      int priority = PRIORITY_DEFAULT);

  /*! Register a callback event for every memory access matching the type
   * bitfield made by the instructions.
   *
   * @param[in] type       A mode bitfield: either QBDI::MEMORY_READ,
   *                       QBDI::MEMORY_WRITE or both (QBDI::MEMORY_READ_WRITE).
   * @param[in] cbk        A lambda function to the callback
   * @param[in] priority   The priority of the callback.
   *
   * @return The id of the registered instrumentation
   * (or VMError::INVALID_EVENTID in case of failure).
   */
  QBDI_EXPORT uint32_t addMemAccessCB(MemoryAccessType type,
                                      const InstCbLambda &cbk,
                                      int priority = PRIORITY_DEFAULT);
  QBDI_EXPORT uint32_t addMemAccessCB(MemoryAccessType type, InstCbLambda &&cbk,
                                      int priority = PRIORITY_DEFAULT);

  /*! Add a virtual callback which is triggered for any memory access at a
   * specific address matching the access type. Virtual callbacks are called via
   * callback forwarding by a gate callback triggered on every memory access.
   * This incurs a high performance cost. The callback has the default priority.
   *
   * @param[in] address  Code address which will trigger the callback.
   * @param[in] type     A mode bitfield: either QBDI::MEMORY_READ,
   *                     QBDI::MEMORY_WRITE or both (QBDI::MEMORY_READ_WRITE).
   * @param[in] cbk      A function pointer to the callback.
   * @param[in] data     User defined data passed to the callback.
   *
   * @return The id of the registered instrumentation
   * (or VMError::INVALID_EVENTID in case of failure).
   */
  QBDI_EXPORT uint32_t addMemAddrCB(rword address, MemoryAccessType type,
                                    InstCallback cbk, void *data);

  /*! Add a virtual callback which is triggered for any memory access at a
   * specific address matching the access type. Virtual callbacks are called via
   * callback forwarding by a gate callback triggered on every memory access.
   * This incurs a high performance cost. The callback has the default priority.
   *
   * @param[in] address  Code address which will trigger the callback.
   * @param[in] type     A mode bitfield: either QBDI::MEMORY_READ,
   *                     QBDI::MEMORY_WRITE or both (QBDI::MEMORY_READ_WRITE).
   * @param[in] cbk      A lambda function to the callback
   *
   * @return The id of the registered instrumentation
   * (or VMError::INVALID_EVENTID in case of failure).
   */
  QBDI_EXPORT uint32_t addMemAddrCB(rword address, MemoryAccessType type,
                                    const InstCbLambda &cbk);
  QBDI_EXPORT uint32_t addMemAddrCB(rword address, MemoryAccessType type,
                                    InstCbLambda &&cbk);

  /*! Add a virtual callback which is triggered for any memory access in a
   * specific address range matching the access type. Virtual callbacks are
   * called via callback forwarding by a gate callback triggered on every memory
   * access. This incurs a high performance cost. The callback has the default
   * priority.
   *
   * @param[in] start    Start of the address range which will trigger the
   *                     callback.
   * @param[in] end      End of the address range which will trigger the
   *                     callback.
   * @param[in] type     A mode bitfield: either QBDI::MEMORY_READ,
   *                     QBDI::MEMORY_WRITE or both (QBDI::MEMORY_READ_WRITE).
   * @param[in] cbk      A function pointer to the callback.
   * @param[in] data     User defined data passed to the callback.
   *
   * @return The id of the registered instrumentation (or
   * VMError::INVALID_EVENTID in case of failure).
   */
  QBDI_EXPORT uint32_t addMemRangeCB(rword start, rword end,
                                     MemoryAccessType type, InstCallback cbk,
                                     void *data);

  /*! Add a virtual callback which is triggered for any memory access in a
   * specific address range matching the access type. Virtual callbacks are
   * called via callback forwarding by a gate callback triggered on every memory
   * access. This incurs a high performance cost. The callback has the default
   * priority.
   *
   * @param[in] start    Start of the address range which will trigger the
   *                     callback.
   * @param[in] end      End of the address range which will trigger the
   *                     callback.
   * @param[in] type     A mode bitfield: either QBDI::MEMORY_READ,
   *                     QBDI::MEMORY_WRITE or both (QBDI::MEMORY_READ_WRITE).
   * @param[in] cbk      A lambda function to the callback
   *
   * @return The id of the registered instrumentation (or
   * VMError::INVALID_EVENTID in case of failure).
   */
  QBDI_EXPORT uint32_t addMemRangeCB(rword start, rword end,
                                     MemoryAccessType type,
                                     const InstCbLambda &cbk);
  QBDI_EXPORT uint32_t addMemRangeCB(rword start, rword end,
                                     MemoryAccessType type, InstCbLambda &&cbk);

  /*! Register a callback event for a specific VM event.
   *
   * @param[in] mask  A mask of VM event type which will trigger the callback.
   * @param[in] cbk   A function pointer to the callback.
   * @param[in] data  User defined data passed to the callback.
   *
   * @return The id of the registered instrumentation (or
   * VMError::INVALID_EVENTID in case of failure).
   */
  QBDI_EXPORT uint32_t addVMEventCB(VMEvent mask, VMCallback cbk, void *data);

  /*! Register a callback event for a specific VM event.
   *
   * @param[in] mask  A mask of VM event type which will trigger the callback.
   * @param[in] cbk   A lambda function to the callback.
   *
   * @return The id of the registered instrumentation (or
   * VMError::INVALID_EVENTID in case of failure).
   */
  QBDI_EXPORT uint32_t addVMEventCB(VMEvent mask, const VMCbLambda &cbk);
  QBDI_EXPORT uint32_t addVMEventCB(VMEvent mask, VMCbLambda &&cbk);

  /*! Remove an instrumentation.
   *
   * @param[in] id The id of the instrumentation to remove.
   *
   * @return  True if instrumentation has been removed.
   */
  QBDI_EXPORT bool deleteInstrumentation(uint32_t id);

  /*! Remove all the registered instrumentations.
   *
   */
  QBDI_EXPORT void deleteAllInstrumentations();

  /*! Obtain the analysis of the current instruction. Analysis results are
   * cached in the VM. The validity of the returned pointer is only guaranteed
   * until the end of the callback, else a deepcopy of the structure is
   * required. This method must only be used in an InstCallback.
   *
   * @param[in] [type] Properties to retrieve during analysis.
   *                   This argument is optional, defaulting to
   *                   QBDI::ANALYSIS_INSTRUCTION | QBDI::ANALYSIS_DISASSEMBLY.
   *
   * @return A InstAnalysis structure containing the analysis result.
   */
  QBDI_EXPORT const InstAnalysis *getInstAnalysis(
      AnalysisType type = ANALYSIS_INSTRUCTION | ANALYSIS_DISASSEMBLY) const;

  /*! Obtain the analysis of a cached instruction. Analysis results are cached
   * in the VM. The validity of the returned pointer is only guaranteed until
   * the end of the callback or a call to a noconst method of the VM object.
   *
   * @param[in] address The address of the instruction to analyse.
   * @param[in] [type]  Properties to retrieve during analysis.
   *                    This argument is optional, defaulting to
   *                    QBDI::ANALYSIS_INSTRUCTION | QBDI::ANALYSIS_DISASSEMBLY
   *
   * @return A InstAnalysis structure containing the analysis result.
   *    null if the instruction isn't in the cache.
   */
  QBDI_EXPORT const InstAnalysis *getCachedInstAnalysis(
      rword address,
      AnalysisType type = ANALYSIS_INSTRUCTION | ANALYSIS_DISASSEMBLY) const;

  /*! Add instrumentation rules to log memory access using inline
   * instrumentation and instruction shadows.
   *
   * @param[in] type Memory mode bitfield to activate the logging for:
   *            either QBDI::MEMORY_READ, QBDI::MEMORY_WRITE or both
   *            (QBDI::MEMORY_READ_WRITE).
   *
   * @return True if inline memory logging is supported, False if not or in case
   *         of error.
   */
  QBDI_EXPORT bool recordMemoryAccess(MemoryAccessType type);

  /*! Obtain the memory accesses made by the last executed instruction.
   *  The method should be called in an InstCallback.
   *
   * @return List of memory access made by the instruction.
   */
  QBDI_EXPORT std::vector<MemoryAccess> getInstMemoryAccess() const;

  /*! Obtain the memory accesses made by the last executed basic block.
   *  The method should be called in a VMCallback with VMEvent::SEQUENCE_EXIT.
   *
   * @return List of memory access made by the instruction.
   */
  QBDI_EXPORT std::vector<MemoryAccess> getBBMemoryAccess() const;

  /*! Pre-cache a known basic block
   *  This method mustn't be called if the VM already runs.
   *
   * @param[in] pc   Start address of a basic block
   *
   * @return True if basic block has been inserted in cache.
   */
  QBDI_EXPORT bool precacheBasicBlock(rword pc);

  /*! Clear a specific address range from the translation cache.
   *
   * @param[in] start Start of the address range to clear from the cache.
   * @param[in] end   End of the address range to clear from the cache.
   *
   */
  QBDI_EXPORT void clearCache(rword start, rword end);

  /*! Clear the entire translation cache.
   */
  QBDI_EXPORT void clearAllCache();
};

} // namespace QBDI

#endif // QBDI_VM_H_
