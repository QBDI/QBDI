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
#ifndef _VM_H_
#define _VM_H_

#include <string>
#include <utility>
#include <vector>
#include <cstdarg>
#include <memory>

#include "Platform.h"
#include "Callback.h"
#include "Errors.h"
#include "State.h"
#include "InstAnalysis.h"

namespace QBDI {

// Forward declaration of engine class
class Engine;
// Foward declaration of (currently) private InstrRule
class InstrRule;
// Forward declaration of private memCBInfo
struct MemCBInfo;

class QBDI_EXPORT VM {
private:
    // Private internal engine
    std::unique_ptr<Engine> engine;
    uint8_t  memoryLoggingLevel;
    std::unique_ptr<std::vector<std::pair<uint32_t, MemCBInfo>>> memCBInfos;
    uint32_t memCBID;
    uint32_t memReadGateCBID;
    uint32_t memWriteGateCBID;

public:
    /*! Construct a new VM for a given CPU with specific attributes
     *
     * @param[in] cpu    The name of the CPU
     * @param[in] mattrs A list of additional attributes
     */
    VM(const std::string& cpu = "", const std::vector<std::string>& mattrs = {});

    ~VM();

    /*! Move constructors.
     *  All the cache is keep.
     *  All registered callbacks will be called with the new pointer of the VM.
     *
     * @param[in] vm     The VM to move
     */
    VM(VM&& vm);

    /*! Move assignment operator
     *  All the cache is keep.
     *  All registered callbacks will be called with the new pointer of the VM.
     *
     * @param[in] vm     The VM to move
     */
    VM& operator=(VM&& vm);

    // delete const move constructor and const move assignment operator
    VM(const VM&& vm) = delete;
    VM& operator=(const VM&& vm) = delete;

    // delete const copy constructor and copy assignment operator
    VM(const VM& vm) = delete;
    VM& operator=(const VM& vm) = delete;


    /*! Obtain the current general purpose register state.
     *
     * @return A structure containing the GPR state.
     */
    GPRState*     getGPRState() const;

    /*! Obtain the current floating point register state.
     *
     * @return A structure containing the FPR state.
     */
    FPRState*     getFPRState() const;

    /*! Set the GPR state
     *
     * @param[in] gprState A structure containing the GPR state.
     */
    void        setGPRState(GPRState* gprState);

    /*! Set the FPR state
     *
     * @param[in] fprState A structure containing the FPR state.
     */
    void        setFPRState(FPRState* fprState);

    /*! Add an address range to the set of instrumented address ranges.
     *
     * @param[in] start  Start address of the range (included).
     * @param[in] end    End address of the range (excluded).
     */
    void         addInstrumentedRange(rword start, rword end);

    /*! Add the executable address ranges of a module to the set of instrumented address ranges.
     *
     * @param[in] name  The module's name.
     *
     * @return  True if at least one range was added to the instrumented ranges.
     */
    bool         addInstrumentedModule(const std::string& name);

    /*! Add the executable address ranges of a module to the set of instrumented address ranges
     * using an address belonging to the module.
     *
     * @param[in] addr  An address contained by module's range.
     *
     * @return  True if at least one range was added to the instrumented ranges.
     */
    bool         addInstrumentedModuleFromAddr(rword addr);

    /*! Adds all the executable memory maps to the instrumented range set.
     * @return  True if at least one range was added to the instrumented ranges.
     */
    bool         instrumentAllExecutableMaps();

    /*! Remove an address range from the set of instrumented address ranges.
     *
     * @param[in] start  Start address of the range (included).
     * @param[in] end    End address of the range (excluded).
     */
    void         removeInstrumentedRange(rword start, rword end);

    /*! Remove the executable address ranges of a module from the set of instrumented address ranges.
     *
     * @param[in] name  The module's name.
     *
     * @return  True if at least one range was removed from the instrumented ranges.
     */
    bool         removeInstrumentedModule(const std::string& name);

    /*! Remove the executable address ranges of a module from the set of instrumented address ranges
     * using an address belonging to the module.
     *
     * @param[in] addr  An address contained by module's range.
     *
     * @return  True if at least one range was removed from the instrumented ranges.
     */
    bool         removeInstrumentedModuleFromAddr(rword addr);

    /*! Remove all instrumented ranges.
     */
    void         removeAllInstrumentedRanges();

    /*! Start the execution by the DBI.
     *
     * @param[in] start  Address of the first instruction to execute.
     * @param[in] stop   Stop the execution when this instruction is reached.
     *
     * @return  True if at least one block has been executed.
     */
    bool        run(rword start, rword stop);

    /*! Call a function using the DBI (and its current state).
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
     *
     */
    bool        call(rword* retval, rword function, const std::vector<rword>& args = {});

    /*! Call a function using the DBI (and its current state).
     *
     * @param[in] [retval]   Pointer to the returned value (optional).
     * @param[in] function   Address of the function start instruction.
     * @param[in] argNum     The number of arguments in the array of arguments.
     * @param[in] args       An array of arguments.
     *
     * @return  True if at least one block has been executed.
     */
    bool        callA(rword* retval, rword function, uint32_t argNum, const rword* args);

    /*! Call a function using the DBI (and its current state).
     *
     * @param[in] [retval]   Pointer to the returned value (optional).
     * @param[in] function   Address of the function start instruction.
     * @param[in] argNum     The number of arguments in the variadic list.
     * @param[in] ap         An stdarg va_list object.
     *
     * @return  True if at least one block has been executed.
     */
    bool        callV(rword* retval, rword function, uint32_t argNum, va_list ap);

    /*! Add a custom instrumentation rule to the VM. Requires internal headers
     *
     * @param[in] rule  A custom instrumentation rule.
     *
     * @return The id of the registered instrumentation (or VMError::INVALID_EVENTID
     * in case of failure).
     */
    uint32_t addInstrRule(InstrRule rule);

    /*! Register a callback event if the instruction matches the mnemonic.
     *
     * @param[in] mnemonic   Mnemonic to match.
     * @param[in] pos        Relative position of the event callback (PREINST / POSTINST).
     * @param[in] cbk        A function pointer to the callback.
     * @param[in] data       User defined data passed to the callback.
     *
     * @return The id of the registered instrumentation (or VMError::INVALID_EVENTID
     * in case of failure).
     */
    uint32_t addMnemonicCB(const char* mnemonic, InstPosition pos, InstCallback cbk, void *data);

    /*! Register a callback event for every instruction executed.
     *
     * @param[in] pos   Relative position of the event callback (PREINST / POSTINST).
     * @param[in] cbk   A function pointer to the callback.
     * @param[in] data  User defined data passed to the callback.
     *
     * @return The id of the registered instrumentation (or VMError::INVALID_EVENTID
     * in case of failure).
     */
    uint32_t    addCodeCB(InstPosition pos, InstCallback cbk, void *data);

     /*! Register a callback for when a specific address is executed.
     *
     * @param[in] address  Code address which will trigger the callback.
     * @param[in] pos      Relative position of the callback (PREINST / POSTINST).
     * @param[in] cbk      A function pointer to the callback.
     * @param[in] data     User defined data passed to the callback.
     *
     * @return The id of the registered instrumentation (or VMError::INVALID_EVENTID
     * in case of failure).
     */
    uint32_t    addCodeAddrCB(rword address, InstPosition pos, InstCallback cbk, void *data);

    /*! Register a callback for when a specific address range is executed.
     *
     * @param[in] start    Start of the address range which will trigger the callback.
     * @param[in] end      End of the address range which will trigger the callback.
     * @param[in] pos     Relative position of the callback (PREINST / POSTINST).
     * @param[in] cbk     A function pointer to the callback.
     * @param[in] data    User defined data passed to the callback.
     *
     * @return The id of the registered instrumentation (or VMError::INVALID_EVENTID
     * in case of failure).
     */
    uint32_t    addCodeRangeCB(rword start, rword end, InstPosition pos, InstCallback cbk, void *data);

    /*! Register a callback event for every memory access matching the type bitfield made by the instructions.
     *
     * @param[in] type       A mode bitfield: either QBDI::MEMORY_READ, QBDI::MEMORY_WRITE or both
     *                       (QBDI::MEMORY_READ_WRITE).
     * @param[in] cbk        A function pointer to the callback.
     * @param[in] data       User defined data passed to the callback.
     *
     * @return The id of the registered instrumentation (or VMError::INVALID_EVENTID
     * in case of failure).
     */
    uint32_t    addMemAccessCB(MemoryAccessType type, InstCallback cbk, void *data);

    /*! Add a virtual callback which is triggered for any memory access at a specific address
     *  matching the access type. Virtual callbacks are called via callback forwarding by a
     *  gate callback triggered on every memory access. This incurs a high performance cost.
     *
     * @param[in] address  Code address which will trigger the callback.
     * @param[in] type     A mode bitfield: either QBDI::MEMORY_READ, QBDI::MEMORY_WRITE or both
     *                     (QBDI::MEMORY_READ_WRITE).
     * @param[in] cbk      A function pointer to the callback.
     * @param[in] data     User defined data passed to the callback.
     *
     * @return The id of the registered instrumentation (or VMError::INVALID_EVENTID
     * in case of failure).
     */
    uint32_t    addMemAddrCB(rword address, MemoryAccessType type, InstCallback cbk, void *data);

    /*! Add a virtual callback which is triggered for any memory access in a specific address range
     *  matching the access type. Virtual callbacks are called via callback forwarding by a
     *  gate callback triggered on every memory access. This incurs a high performance cost.
     *
     * @param[in] start    Start of the address range which will trigger the callback.
     * @param[in] end      End of the address range which will trigger the callback.
     * @param[in] type     A mode bitfield: either QBDI::MEMORY_READ, QBDI::MEMORY_WRITE or both
     *                     (QBDI::MEMORY_READ_WRITE).
     * @param[in] cbk      A function pointer to the callback.
     * @param[in] data     User defined data passed to the callback.
     *
     * @return The id of the registered instrumentation (or VMError::INVALID_EVENTID
     * in case of failure).
     */
    uint32_t    addMemRangeCB(rword start, rword end, MemoryAccessType type, InstCallback cbk, void *data);


    /*! Register a callback event for a specific VM event.
     *
     * @param[in] mask  A mask of VM event type which will trigger the callback.
     * @param[in] cbk   A function pointer to the callback.
     * @param[in] data  User defined data passed to the callback.
     *
     * @return The id of the registered instrumentation (or VMError::INVALID_EVENTID
     * in case of failure).
     */
    uint32_t    addVMEventCB(VMEvent mask, VMCallback cbk, void *data);

   /*! Remove an instrumentation.
     *
     * @param[in] id The id of the instrumentation to remove.
     *
     * @return  True if instrumentation has been removed.
     */
    bool        deleteInstrumentation(uint32_t id);

    /*! Remove all the registered instrumentations.
     *
     */
    void        deleteAllInstrumentations();

    /*! Obtain the analysis of an instruction metadata. Analysis results are cached in the VM.
     *  The validity of the returned pointer is only guaranteed until the end of the callback, else
     *  a deepcopy of the structure is required.
     *
     * @param[in] [type]         Properties to retrieve during analysis.
     *                           This argument is optional, defaulting to
     *                           QBDI::ANALYSIS_INSTRUCTION | QBDI::ANALYSIS_DISASSEMBLY.
     *
     * @return A InstAnalysis structure containing the analysis result.
     */
    const InstAnalysis* getInstAnalysis(AnalysisType type = ANALYSIS_INSTRUCTION | ANALYSIS_DISASSEMBLY);

    /*! Add instrumentation rules to log memory access using inline instrumentation and
     *  instruction shadows.
     *
     * @param[in] type Memory mode bitfield to activate the logging for: either QBDI::MEMORY_READ,
     *                 QBDI::MEMORY_WRITE or both (QBDI::MEMORY_READ_WRITE).
     *
     * @return True if inline memory logging is supported, False if not or in case of error.
     */
    bool recordMemoryAccess(MemoryAccessType type);

    /*! Obtain the memory accesses made by the last executed instruction.
     *  The method should be called in an InstCallback.
     *
     * @return List of memory access made by the instruction.
     */
    std::vector<MemoryAccess> getInstMemoryAccess() const;

    /*! Obtain the memory accesses made by the last executed basic block.
     *  The method should be called in a VMCallback with VMEvent::SEQUENCE_EXIT.
     *
     * @return List of memory access made by the instruction.
     */
    std::vector<MemoryAccess> getBBMemoryAccess() const;

    /*! Pre-cache a known basic block
     *
     * @param[in] pc   Start address of a basic block
     *
     * @return True if basic block has been inserted in cache.
     */
    bool precacheBasicBlock(rword pc);

    /*! Clear a specific address range from the translation cache.
     *
     * @param[in] start Start of the address range to clear from the cache.
     * @param[in] end   End of the address range to clear from the cache.
     *
    */
    void clearCache(rword start, rword end);

    /*! Clear the entire translation cache.
    */
    void clearAllCache();

};

} // QBDI::

#endif // _VM_H_
