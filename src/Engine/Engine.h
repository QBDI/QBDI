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
#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include <memory>
#include <vector>
#include <utility>
#include <cstdlib>

#if !defined(__APPLE__)
#include <malloc.h>
#endif

#include "Callback.h"
#include "InstAnalysis.h"
#include "State.h"
#include "Range.h"

namespace llvm {
    class MCAsmInfo;
    class MCCodeEmitter;
    class MCContext;
    class MCInstrInfo;
    class MCObjectFileInfo;
    class MCRegisterInfo;
    class MCSubtargetInfo;
}

namespace QBDI {

class Assembly;
class ExecBlock;
class ExecBlockManager;
class ExecBroker;
class PatchRule;
class InstrRule;
class Patch;
class InstMetadata;

struct CallbackRegistration {
    VMEvent    mask;
    VMCallback cbk;
    void*      data;
};

class Engine {
private:

    std::unique_ptr<llvm::MCAsmInfo>         MAI;
    std::unique_ptr<llvm::MCCodeEmitter>     MCE;
    std::unique_ptr<llvm::MCContext>         MCTX;
    std::unique_ptr<llvm::MCInstrInfo>       MCII;
    std::unique_ptr<llvm::MCObjectFileInfo>  MOFI;
    std::unique_ptr<llvm::MCRegisterInfo>    MRI;
    std::unique_ptr<llvm::MCSubtargetInfo>   MSTI;
    std::string                              tripleName;
    std::string                              cpu;
    std::vector<std::string>                 mattrs;
    VMInstanceRef                            vminstance;

    std::unique_ptr<Assembly>                                       assembly;
    std::unique_ptr<ExecBlockManager>                               blockManager;
    std::unique_ptr<ExecBroker>                                     execBroker;
    std::vector<PatchRule>                                          patchRules;
    std::vector<std::pair<uint32_t, std::unique_ptr<InstrRule>>>    instrRules;
    uint32_t                                                        instrRulesCounter;
    std::vector<std::pair<uint32_t, CallbackRegistration>>          vmCallbacks;
    uint32_t                                                        vmCallbacksCounter;
    std::unique_ptr<GPRState>                                       gprState;
    std::unique_ptr<FPRState>                                       fprState;
    GPRState*                                                       curGPRState;
    FPRState*                                                       curFPRState;
    ExecBlock*                                                      curExecBlock;

    void init();
    void reinit(const std::string& cpu, const std::vector<std::string>& mattrs);

    std::vector<Patch> patch(rword start);

    void initGPRState();
    void initFPRState();

    void instrument(std::vector<Patch> &basicBlock);
    void handleNewBasicBlock(rword pc);

    void signalEvent(VMEvent kind, rword currentBasicBlock, GPRState *gprState, FPRState *fprState);

public:

    /*! Construct a new Engine for a given CPU with specific attributes
     *
     * @param[in] cpu        The name of the CPU
     * @param[in] mattrs     A list of additional attributes
     * @param[in] vminstance Pointer to public engine interface
     */
    Engine(const std::string& cpu = "", const std::vector<std::string>& mattrs = {}, VMInstanceRef vminstance = nullptr);

    ~Engine();

    Engine(const Engine&&) = delete;
    Engine& operator=(const Engine&&) = delete;

    Engine(const Engine&);
    Engine& operator=(const Engine&);

    /*! Change the pointer to vminstance. The new pointer will be used for
     * future callback
     *
     * @param[in] vminstance   The new vminstance
     */
    void changeVMInstanceRef(VMInstanceRef vminstance);

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
    void        setGPRState(const GPRState* gprState);

    /*! Set the GPR state
     *
     * @param[in] fprState A structure containing the FPR state.
     */
    void        setFPRState(const FPRState* fprState);

    /*! Add an address range to the set of instrumented address ranges.
     *
     * @param[in] start  Start address of the range (included).
     * @param[in] end    End address of the range (excluded).
     */
    void         addInstrumentedRange(rword start, rword end);

    /*! Add the executable address ranges of a module to the set of instrumented address ranges.
     *
     * @param[in] name  The module's name.
     * @return  True if at least one range was added to the instrumented ranges.
     */
    bool         addInstrumentedModule(const std::string& name);

    /*! Add the executable address ranges of a module to the set of instrumented address ranges.
     * using an address belonging to the module.
     *
     * @param[in] addr  An address contained by module's range.
     * @return  True if at least one range was added to the instrumented ranges.
     */
    bool         addInstrumentedModuleFromAddr(rword addr);

    /*! Adds all the executable memory maps to the instrumented range set.
     * @return  True if at least one range was added to the instrumented ranges.
     */
    bool         instrumentAllExecutableMaps();

    /*! Remove an address range to the set from instrumented address ranges.
     *
     * @param[in] start  Start address of the range (included).
     * @param[in] end    End address of the range (excluded).
     */
    void         removeInstrumentedRange(rword start, rword end);

    /*! Remove the executable address ranges of a module from the set of instrumented address ranges.
     *
     * @param[in] name  The module's name.
     * @return  True if at least one range was removed to the instrumented ranges.
     */
    bool         removeInstrumentedModule(const std::string& name);

    /*! Remove the executable address ranges of a module from the set of instrumented address ranges.
     * using an address belonging to the module.
     *
     * @param[in] addr  An address contained by module's range.
     * @return  True if at least one range was removed to the instrumented ranges.
     */
    bool         removeInstrumentedModuleFromAddr(rword addr);

    /*! Remove all instrumented ranges.
     */
    void removeAllInstrumentedRanges();

    /*! Start the execution by the DBI.
     *
     * @param[in] start  Pointer to the first instruction to execute.
     * @param[in] stop   Stop the execution when this instruction is reached.
     * @return  True if at least one block has been executed.
     */
    bool        run(rword start, rword stop);

    /*! Add a custom instrumentation rule to the engine. Requires internal headers
     *
     * @param[in] rule A custom instrumentation rule.
     * @param[in] top_list Place the rule before all existing rules of its category
     *
     * @return The id of the registered instrumentation (or VMError::INVALID_EVENTID
     * in case of failure).
     */
    uint32_t addInstrRule(std::unique_ptr<InstrRule>&& rule);

    /*! Register a callback event for a specific VM event.
     *
     * @param[in] mask A mask of VM event type which will trigger the callback.
     * @param[in] cbk  A function pointer to the callback.
     * @param[in] data User defined data passed to the callback.
     *
     * @return The id of the registered instrumentation (or VMError::INVALID_EVENTID
     * in case of failure).
     */
    uint32_t    addVMEventCB(VMEvent mask, VMCallback cbk, void *data);

    /*! Remove an instrumentation.
     *
     * @param[in] id The id of the instrumentation to remove.
     * @return  True if instrumentation has been removed.
     */
    bool        deleteInstrumentation(uint32_t id);

    /*! Remove all the registered instrumentations.
     *
     */
    void        deleteAllInstrumentations();

    /*! Expose current ExecBlock
     *
     * @return A pointer to current ExecBlock
     */
    const ExecBlock* getCurExecBlock() const { return curExecBlock; }

    /*! Check if current ExecBlock is PREINST in current state
     *
     * @return true if engine state is Pre-inst
     */
    bool isPreInst() const;

    /*! Pre-cache a known basic block.
     * The VM mustn't run when called.
     *
     * @param[in] pc Start address of a basic block
     *
     * @return True if basic block has been inserted in cache.
     */
    bool precacheBasicBlock(rword pc);

    /*! Return an InstAnalysis for a cached instruction.
     * The pointer may be invalid by any noconst method call.
     *
     * @param[in] address Start address of the instruction
     * @param[in] type    type of the Analysis
     *
     * @return A pointer to the Analysis or a null pointer if the instruction isn't
     *    in the cache.
     */
    const InstAnalysis* getInstAnalysis(rword address, AnalysisType type) const;

    /*! Clear a specific address range from the translation cache.
     *
     * @param[in] start Start of the address range to clear from the cache.
     * @param[in] end   End of the address range to clear from the cache.
     *
    */
    void clearCache(rword start, rword end);

    /*! Clear a specific address rangeSet from the translation cache.
     *
     * @param[in] rangeSet    The range set to clear from the cache.
    */
    void clearCache(RangeSet<rword> rangeSet);

    /*! Clear the entire translation cache.
    */
    void clearAllCache();
};

} // QBDI::

#endif // ENGINE_H
