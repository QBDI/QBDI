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
#ifndef _VM_C_H_
#define _VM_C_H_

#include <stdlib.h>
#include <stdarg.h>

#include "Platform.h"
#include "Callback.h"
#include "Errors.h"
#include "State.h"
#include "InstAnalysis.h"

#ifdef __cplusplus
namespace QBDI {
extern "C" {
#endif


/*! Create and initialize a VM instance.
 *
 * @param[out] instance VM instance created.
 * @param[in]  cpu      A C string naming the CPU model to use. If NULL, the default architecture
 *                      CPU model is used (see LLVM documentation for more details).
 * @param[in]  mattrs   A NULL terminated array of C strings specifying the attributes of the cpu
 *                      model. If NULL, no additional features are specified.
 */
QBDI_EXPORT void qbdi_initVM(VMInstanceRef* instance, const char* cpu, const char** mattrs);

/*! Destroy an instance of VM.
 *
 * @param[in] instance VM instance.
 */
QBDI_EXPORT void qbdi_terminateVM(VMInstanceRef instance);

/*! Add an address range to the set of instrumented address ranges.
 *
 * @param[in] instance VM instance.
 * @param[in] start  Start address of the range (included).
 * @param[in] end    End address of the range (excluded).
 */
QBDI_EXPORT void qbdi_addInstrumentedRange(VMInstanceRef instance, rword start, rword end);

/*! Add the executable address ranges of a module to the set of instrumented address ranges.
 *
 * @param[in] instance VM instance.
 * @param[in] name  The module's name.
 *
 * @return  True if at least one range was added to the instrumented ranges.
 */
QBDI_EXPORT bool qbdi_addInstrumentedModule(VMInstanceRef instance, const char* name);

/*! Add the executable address ranges of a module to the set of instrumented address ranges
 * using an address belonging to the module.
 *
 * @param[in] instance  VM instance.
 * @param[in] addr      An address contained by module's range.
 *
 * @return  True if at least one range was added to the instrumented ranges.
 */
QBDI_EXPORT bool qbdi_addInstrumentedModuleFromAddr(VMInstanceRef instance, rword addr);

/*! Adds all the executable memory maps to the instrumented range set.
 *
 * @param[in] instance VM instance.
 *
 * @return  True if at least one range was added to the instrumented ranges.
 */
QBDI_EXPORT bool qbdi_instrumentAllExecutableMaps(VMInstanceRef instance);

/*! Remove an address range from the set of instrumented address ranges.
 *
 * @param[in] instance  VM instance.
 * @param[in] start     Start address of the range (included).
 * @param[in] end       End address of the range (excluded).
 */
QBDI_EXPORT void qbdi_removeInstrumentedRange(VMInstanceRef instance, rword start, rword end);

/*! Remove the executable address ranges of a module from the set of instrumented address ranges.
 *
 * @param[in] instance  VM instance.
 * @param[in] name      The module's name.
 *
 * @return  True if at least one range was removed from the instrumented ranges.
 */
QBDI_EXPORT bool qbdi_removeInstrumentedModule(VMInstanceRef instance, const char* name);

/*! Remove the executable address ranges of a module from the set of instrumented address ranges
 * using an address belonging to the module.
 *
 * @param[in] instance  VM instance.
 * @param[in] addr      An address contained by module's range.
 *
 * @return  True if at least one range was removed from the instrumented ranges.
 */
QBDI_EXPORT bool qbdi_removeInstrumentedModuleFromAddr(VMInstanceRef instance, rword addr);

/*! Remove all instrumented ranges.
 *
 * @param[in] instance  VM instance.
 */
QBDI_EXPORT void qbdi_removeAllInstrumentedRanges(VMInstanceRef instance);

/*! Start the execution by the DBI from a given address (and stop when another is reached).
 *
 * @param[in] instance  VM instance.
 * @param[in] start     Address of the first instruction to execute.
 * @param[in] stop      Stop the execution when this instruction is reached.
 *
 * @return  True if at least one block has been executed.
 */
QBDI_EXPORT bool qbdi_run(VMInstanceRef instance, rword start, rword stop);

/*! Call a function using the DBI (and its current state).
 *
 * @param[in] instance   VM instance.
 * @param[in] [retval]   Pointer to the returned value (optional).
 * @param[in] function   Address of the function start instruction.
 * @param[in] argNum     The number of arguments in the variadic list.
 * @param[in] ...        A variadic list of arguments.
 *
 * @return  True if at least one block has been executed.
 *
 * @details Example:
 *
 *     // perform (with QBDI) a call similar to (*funcPtr)(42);
 *     uint8_t *fakestack = NULL;
 *     VMInstanceRef vm;
 *     qbdi_initVM(&vm, NULL, NULL);
 *     GPRState* gprState = qbdi_getGPRState(vm);
 *     qbdi_allocateVirtualStack(gprState, 0x1000000, &fakestack);
 *     qbdi_addInstrumentedModuleFromAddr(vm, funcPtr);
 *     rword retVal;
 *     qbdi_call(vm, &retVal, funcPtr, 1, 42);
 *
 */
QBDI_EXPORT bool qbdi_call(VMInstanceRef instance, rword* retval, rword function, uint32_t argNum, ...);

/*! Call a function using the DBI (and its current state).
 *
 * @param[in] instance   VM instance.
 * @param[in] [retval]   Pointer to the returned value (optional).
 * @param[in] function   Address of the function start instruction.
 * @param[in] argNum     The number of arguments in the variadic list.
 * @param[in] ap         An stdarg va_list object.
 *
 * @return  True if at least one block has been executed.
 */
QBDI_EXPORT bool qbdi_callV(VMInstanceRef instance, rword* retval, rword function, uint32_t argNum, va_list ap);

/*! Call a function using the DBI (and its current state).
 *
 * @param[in] instance   VM instance.
 * @param[in] [retval]   Pointer to the returned value (optional).
 * @param[in] function   Address of the function start instruction.
 * @param[in] argNum     The number of arguments in the variadic list.
 * @param[in] args       Arguments as an array of rword values.
 *
 * @return  True if at least one block has been executed.
 */
QBDI_EXPORT bool qbdi_callA(VMInstanceRef instance, rword* retval, rword function, uint32_t argNum, const rword* args);

/*! Obtain the current general purpose register state.
 *
 * @param[in] instance  VM instance.
 *
 * @return              A structure containing the General Purpose Registers state.
 */
QBDI_EXPORT GPRState* qbdi_getGPRState(VMInstanceRef instance);

/*! Obtain the current floating point register state.
 *
 * @param[in] instance  VM instance.
 *
 * @return              A structure containing the Floating Point Registers state.
 */
QBDI_EXPORT FPRState* qbdi_getFPRState(VMInstanceRef instance);

/*! Set the GPR state
 *
 * @param[in] instance  VM instance.
 * @param[in] gprState  A structure containing the General Purpose Registers state.
 */
QBDI_EXPORT void qbdi_setGPRState(VMInstanceRef instance, GPRState* gprState);

/*! Set the GPR state
 *
 * @param[in] instance  VM instance.
 * @param[in] fprState  A structure containing the Floating Point Registers state.
 */
QBDI_EXPORT void qbdi_setFPRState(VMInstanceRef instance, FPRState* fprState);

/*! Add a custom instrumentation rule to the VM.
 *
 * @param[in] instance   VM instance.
 * @param[in] cbk       A function pointer to the callback
 * @param[in] type      Analyse type needed for this instruction function pointer to the callback
 * @param[in] data      User defined data passed to the callback.
 *
 * @return The id of the registered instrumentation (or VMError::INVALID_EVENTID
 * in case of failure).
 */
QBDI_EXPORT uint32_t qbdi_addInstrRule(VMInstanceRef instance, QBDI_InstrumentCallback cbk, AnalysisType type, void* data);

/*! Add a custom instrumentation rule to the VM for a range of address
 *
 * @param[in] instance  VM instance.
 * @param[in] start     Begin of the range of address where apply the rule
 * @param[in] end       End of the range of address where apply the rule
 * @param[in] cbk       A function pointer to the callback
 * @param[in] type      Analyse type needed for this instruction function pointer to the callback
 * @param[in] data      User defined data passed to the callback.
 *
 * @return The id of the registered instrumentation (or VMError::INVALID_EVENTID
 * in case of failure).
 */
QBDI_EXPORT uint32_t qbdi_addInstrRuleRange(VMInstanceRef instance, rword start, rword end, QBDI_InstrumentCallback cbk, AnalysisType type, void* data);

/*! Register a callback event for every memory access matching the type bitfield made by the instructions.
 *
 * @param[in] instance   VM instance.
 * @param[in] type       A mode bitfield: either QBDI_MEMORY_READ, QBDI_MEMORY_WRITE or both (QBDI_MEMORY_READ_WRITE).
 * @param[in] cbk        A function pointer to the callback.
 * @param[in] data       User defined data passed to the callback.
 *
 * @return The id of the registered instrumentation (or QBDI_INVALID_EVENTID
 * in case of failure).
 */
QBDI_EXPORT uint32_t qbdi_addMemAccessCB(VMInstanceRef instance, MemoryAccessType type, InstCallback cbk, void *data);

/*! Add a virtual callback which is triggered for any memory access at a specific address
 *  matching the access type. Virtual callbacks are called via callback forwarding by a
 *  gate callback triggered on every memory access. This incurs a high performance cost.
 *
 * @param[in] instance  VM instance.
 * @param[in] address  Code address which will trigger the callback.
 * @param[in] type     A mode bitfield: either QBDI_MEMORY_READ, QBDI_MEMORY_WRITE or both (QBDI_MEMORY_READ_WRITE).
 * @param[in] cbk      A function pointer to the callback.
 * @param[in] data     User defined data passed to the callback.
 *
 * @return The id of the registered instrumentation (or QBDI_INVALID_EVENTID
 * in case of failure).
 */
QBDI_EXPORT uint32_t qbdi_addMemAddrCB(VMInstanceRef instance, rword address, MemoryAccessType type, InstCallback cbk, void *data);

/*! Add a virtual callback which is triggered for any memory access in a specific address range
 *  matching the access type. Virtual callbacks are called via callback forwarding by a
 *  gate callback triggered on every memory access. This incurs a high performance cost.
 *
 * @param[in] instance  VM instance.
 * @param[in] start    Start of the address range which will trigger the callback.
 * @param[in] end      End of the address range which will trigger the callback.
 * @param[in] type     A mode bitfield: either QBDI_MEMORY_READ, QBDI_MEMORY_WRITE or both (QBDI_MEMORY_READ_WRITE).
 * @param[in] cbk      A function pointer to the callback.
 * @param[in] data     User defined data passed to the callback.
 *
 * @return The id of the registered instrumentation (or QBDI_INVALID_EVENTID
 * in case of failure).
 */
QBDI_EXPORT uint32_t qbdi_addMemRangeCB(VMInstanceRef instance, rword start, rword end, MemoryAccessType type, InstCallback cbk, void *data);

/*! Register a callback event if the instruction matches the mnemonic.
 *
 * @param[in] instance   VM instance.
 * @param[in] mnemonic   Mnemonic to match.
 * @param[in] pos        Relative position of the event callback (QBDI_PREINST / QBDI_POSTINST).
 * @param[in] cbk        A function pointer to the callback.
 * @param[in] data       User defined data passed to the callback.
 *
 * @return The id of the registered instrumentation (or QBDI_INVALID_EVENTID
 * in case of failure).
 */
QBDI_EXPORT uint32_t qbdi_addMnemonicCB(VMInstanceRef instance, const char* mnemonic, InstPosition pos, InstCallback cbk, void *data);

/*! Register a callback event for a specific instruction event.
 *
 * @param[in] instance  VM instance.
 * @param[in] pos       Relative position of the event callback (QBDI_PREINST / QBDI_POSTINST).
 * @param[in] cbk       A function pointer to the callback.
 * @param[in] data      User defined data passed to the callback.
 *
 * @return The id of the registered instrumentation (or QBDI_INVALID_EVENTID
 * in case of failure).
 */
QBDI_EXPORT uint32_t qbdi_addCodeCB(VMInstanceRef instance, InstPosition pos, InstCallback cbk, void *data);

/*! Register a callback for when a specific address is executed.
 *
 * @param[in] instance  VM instance.
 * @param[in] address   Code address which will trigger the callback.
 * @param[in] pos       Relative position of the callback (QBDI_PREINST / QBDI_POSTINST).
 * @param[in] cbk       A function pointer to the callback.
 * @param[in] data      User defined data passed to the callback.
 *
 * @return The id of the registered instrumentation (or QBDI_INVALID_EVENTID
 * in case of failure).
 */
QBDI_EXPORT uint32_t qbdi_addCodeAddrCB(VMInstanceRef instance, rword address, InstPosition pos, InstCallback cbk, void *data);

/*! Register a callback for when a specific address range is executed.
 *
 * @param[in] instance  VM instance.
 * @param[in] start     Start of the address range which will trigger the callback.
 * @param[in] end       End of the address range which will trigger the callback.
 * @param[in] pos       Relative position of the callback (QBDI_PREINST / QBDI_POSTINST).
 * @param[in] cbk       A function pointer to the callback.
 * @param[in] data      User defined data passed to the callback.
 *
 * @return The id of the registered instrumentation (or QBDI_INVALID_EVENTID
 * in case of failure).
 */
QBDI_EXPORT uint32_t qbdi_addCodeRangeCB(VMInstanceRef instance, rword start, rword end, InstPosition pos, InstCallback cbk, void *data);

/*! Register a callback event for a specific VM event.
 *
 * @param[in] instance  VM instance.
 * @param[in] mask      A mask of VM event type which will trigger the callback.
 * @param[in] cbk       A function pointer to the callback.
 * @param[in] data      User defined data passed to the callback.
 *
 * @return The id of the registered instrumentation (or QBDI_INVALID_EVENTID
 * in case of failure).
 */
QBDI_EXPORT uint32_t qbdi_addVMEventCB(VMInstanceRef instance, VMEvent mask, VMCallback cbk, void *data);

/*! Remove an instrumentation.
 *
 * @param[in] instance  VM instance.
 * @param[in] id        The id of the instrumentation to remove.
 *
 * @return  True if instrumentation has been removed.
 */
QBDI_EXPORT bool qbdi_deleteInstrumentation(VMInstanceRef instance, uint32_t id);

/*! Remove all the registered instrumentations.
 *
 * @param[in] instance  VM instance.
 */
QBDI_EXPORT void qbdi_deleteAllInstrumentations(VMInstanceRef instance);

 /*! Obtain the analysis of an instruction metadata. Analysis results are cached in the VM.
 *  The validity of the returned pointer is only guaranteed until the end of the callback, else
 *  a deepcopy of the structure is required.
 *
 * @param[in] instance     VM instance.
 * @param[in] type         Properties to retrieve during analysis.
 *
 * @return A InstAnalysis structure containing the analysis result.
 */
QBDI_EXPORT const InstAnalysis* qbdi_getInstAnalysis(VMInstanceRef instance, AnalysisType type);

/*! Add instrumentation rules to log memory access using inline instrumentation and
 *  instruction shadows.

 * @param[in] instance  VM instance.
 * @param[in] type      Memory mode bitfield to activate the logging for: either QBDI_MEMORY_READ,
 *                      QBDI_MEMORY_WRITE or both (QBDI_MEMORY_READ_WRITE).
 *
 * @return True if inline memory logging is supported, False if not or in case of error.
 */
QBDI_EXPORT bool qbdi_recordMemoryAccess(VMInstanceRef instance, MemoryAccessType type);

/*! Obtain the memory accesses made by the last executed instruction.
 *  The method should be called in an InstCallback.
 *  Return NULL and a size of 0 if the instruction made no memory access.
 *
 *  @param[in]  instance     VM instance.
 *  @param[out] size         Will be set to the number of elements in the returned array.
 *
 * @return An array of memory accesses made by the instruction.
 */
QBDI_EXPORT MemoryAccess* qbdi_getInstMemoryAccess(VMInstanceRef instance, size_t* size);

/*! Obtain the memory accesses made by the last executed basic block.
 *  The method should be called in a VMCallback with QBDI_SEQUENCE_EXIT.
 *  Return NULL and a size of 0 if the basic block made no memory access.
 *
 *  @param[in]  instance     VM instance.
 *  @param[out] size         Will be set to the number of elements in the returned array.
 *
 * @return An array of memory accesses made by the basic block.
 */
QBDI_EXPORT MemoryAccess* qbdi_getBBMemoryAccess(VMInstanceRef instance, size_t* size);

/*! Pre-cache a known basic block
 *
 *  @param[in]  instance     VM instance.
 *  @param[in]  pc           Start address of a basic block
 *
 * @return True if basic block has been inserted in cache.
 */
QBDI_EXPORT bool qbdi_precacheBasicBlock(VMInstanceRef instance, rword pc);

/*! Clear a specific address range from the translation cache.
 *
 * @param[in] instance     VM instance.
 * @param[in] start        Start of the address range to clear from the cache.
 * @param[in] end          End of the address range to clear from the cache.
 *
 */
QBDI_EXPORT void qbdi_clearCache(VMInstanceRef instance, rword start, rword end);

/*! Clear the entire translation cache.
 *
 * @param[in] instance     VM instance.
 */
QBDI_EXPORT void qbdi_clearAllCache(VMInstanceRef instance);

#ifdef __cplusplus
} // "C"
} // QBDI::
#endif

#endif // _VM_C_H_
