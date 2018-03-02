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
#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdbool.h>
#include <stdarg.h>

#include "Platform.h"
#include "State.h"

// C++ Only functions
#ifdef __cplusplus
#include <string>
#include <vector>
#include "Range.h"
#include "Bitmask.h"


namespace QBDI {

/*! Memory access rights.
 */
typedef enum {
    _QBDI_EI(PF_NONE) = 0,   /*!< No access */
    _QBDI_EI(PF_READ) = 1,   /*!< Read access */
    _QBDI_EI(PF_WRITE) = 2,  /*!< Write access */
    _QBDI_EI(PF_EXEC) = 4    /*!< Execution access */
} Permission;

_QBDI_ENABLE_BITMASK_OPERATORS(Permission)

/*! Map of a memory area (region).
 */
class MemoryMap {

public:

    Range<rword> range;       /*!< A range of memory (region), delimited between a start and an (excluded) end address. */
    Permission   permission;  /*!< Region access rights (PF_READ, PF_WRITE, PF_EXEC). */
    std::string  name;        /*!< Region name (useful when a region is mapping a module). */

    /* Construct a new (empty) MemoryMap.
     */
    MemoryMap() : range(0, 0), permission(QBDI::PF_NONE), name("") {};

    /*! Construct a new MemoryMap (given some properties).
     *
     * @param[in] range        A range of memory (region), delimited between
     *                         a start and an (excluded) end address.
     * @param[in] permission   Region access rights (PF_READ, PF_WRITE, PF_EXEC).
     * @param[in] name         Region name (useful when a region is mapping a module).
     */
   MemoryMap(Range<rword> range, Permission permission, std::string name) :
        range(range), permission(permission), name(name) {}

};

/*! Get a list of all the memory maps (regions) of a process.
 *
 * @param[in] pid The identifier of the process.
 *
 * @return  A vector of MemoryMap object.
 */
QBDI_EXPORT std::vector<MemoryMap> getRemoteProcessMaps(QBDI::rword pid);

/*! Get a list of all the memory maps (regions) of the current process.
 *
 * @return  A vector of MemoryMap object.
 */
QBDI_EXPORT std::vector<MemoryMap> getCurrentProcessMaps();

/*! Get a list of all the module names loaded in the process memory.
 *
 * @return  A vector of string of module names.
 */
QBDI_EXPORT std::vector<std::string> getModuleNames();

}
#endif

// C and C++ functions
#ifdef __cplusplus
namespace QBDI {
extern "C" {
#endif


/*! Allocate a block of memory of a specified sized with an aligned base address.
 * 
 * @param[in] size  Allocation size in bytes.
 * @param[in] align Base address alignement in bytes.
 *
 * @return  Pointer to the allocated memory or NULL in case an error was encountered.
 *
*/
QBDI_EXPORT void* qbdi_alignedAlloc(size_t size, size_t align);

/*! Free a block of aligned memory allocated with alignedAlloc.
 * 
 * @param[in] ptr  Pointer to the allocated memory.
 *
*/
QBDI_EXPORT void qbdi_alignedFree(void* ptr);

/*! Get a list of all the module names loaded in the process memory.
 *  If no modules are found, size is set to 0 and this function returns NULL.
 *
 * @param[out] size    Will be set to the number of strings in the returned array.
 *
 * @return  An array of C strings, each one containing the name of a loaded module.
 *          This array needs to be free'd by the caller by repetively calling free() on each of its 
 *          element then finally on the array itself.
 */
QBDI_EXPORT char** qbdi_getModuleNames(size_t* size);

/*! Allocate a new stack and setup the GPRState accordingly.
 *  The allocated stack needs to be freed with alignedFree().
 *
 *  @param[in]  ctx       GPRState which will be setup to use the new stack.
 *  @param[in]  stackSize Size of the stack to be allocated.
 *  @param[out] stack     The newly allocated stack pointer will be returned in the variable 
 *                        pointed by stack.
 *
 *  @return               True if stack allocation was successfull.
 */
QBDI_EXPORT bool qbdi_allocateVirtualStack(GPRState *ctx, uint32_t stackSize, uint8_t **stack);

/*! Simulate a call by modifying the stack and registers accordingly.
 *
 *  @param[in] ctx           GPRState where the simulated call will be setup. The state needs to 
 *                           point to a valid stack for example setup with allocateVirtualStack().
 *  @param[in] returnAddress Return address of the call to simulate.
 *  @param[in] argNum        The number of arguments in the variadic list.
 *  @param[in] ...           A variadic list of arguments.
 */
QBDI_EXPORT void qbdi_simulateCall(GPRState *ctx, rword returnAddress, uint32_t argNum, ...);

/*! Simulate a call by modifying the stack and registers accordingly (stdarg version).
 *
 *  @param[in] ctx           GPRState where the simulated call will be setup. The state needs to 
 *                           point to a valid stack for example setup with allocateVirtualStack().
 *  @param[in] returnAddress Return address of the call to simulate.
 *  @param[in] argNum        The number of arguments in the va_list object.
 *  @param[in] ap            An stdarg va_list object.
 */
QBDI_EXPORT void qbdi_simulateCallV(GPRState *ctx, rword returnAddress, uint32_t argNum, va_list ap);

/*! Simulate a call by modifying the stack and registers accordingly (C array version).
 *
 *  @param[in] ctx           GPRState where the simulated call will be setup. The state needs to 
 *                           point to a valid stack for example setup with allocateVirtualStack().
 *  @param[in] returnAddress Return address of the call to simulate.
 *  @param[in] argNum        The number of arguments in the array args.
 *  @param[in] args          An array or arguments.
 */
QBDI_EXPORT void qbdi_simulateCallA(GPRState *ctx, rword returnAddress, uint32_t argNum, const rword* args);

#ifdef __cplusplus
/*
 * C API C++ bindings
 */

/*! Allocate a block of memory of a specified sized with an aligned base address.
 * 
 * @param[in] size  Allocation size in bytes.
 * @param[in] align Base address alignement in bytes.
 *
 * @return  Pointer to the allocated memory or NULL in case an error was encountered.
 *
*/
inline void* alignedAlloc(size_t size, size_t align) {
    return qbdi_alignedAlloc(size, align);
}

/*! Free a block of aligned memory allocated with alignedAlloc.
 * 
 * @param[in] ptr  Pointer to the allocated memory.
 *
*/
inline void alignedFree(void* ptr) {
    return qbdi_alignedFree(ptr);
}

/*! Get a list of all the module names loaded in the process memory.
 *  If no modules are found, size is set to 0 and this function returns NULL.
 *
 * @param[out] size    Will be set to the number of strings in the returned array.
 *
 * @return  An array of C strings, each one containing the name of a loaded module.
 *          This array needs to be free'd by the caller by repetively calling free() on each of its 
 *          element then finally on the array itself.
 */
inline char** getModuleNames(size_t* size) {
    return qbdi_getModuleNames(size);
}

/*! Allocate a new stack and setup the GPRState accordingly.
 *  The allocated stack needs to be freed with alignedFree().
 *
 *  @param[in]  ctx       GPRState which will be setup to use the new stack.
 *  @param[in]  stackSize Size of the stack to be allocated.
 *  @param[out] stack     The newly allocated stack pointer will be returned in the variable 
 *                        pointed by stack.
 *
 *  @return               True if stack allocation was successfull.
 */
inline bool allocateVirtualStack(GPRState *ctx, uint32_t stackSize, uint8_t **stack) {
    return qbdi_allocateVirtualStack(ctx, stackSize, stack);
}

/*! Simulate a call by modifying the stack and registers accordingly (std::vector version).
 *
 *  @param[in] ctx           GPRState where the simulated call will be setup. The state needs to 
 *                           point to a valid stack for example setup with allocateVirtualStack().
 *  @param[in] returnAddress Return address of the call to simulate.
 *  @param[in] args          A list of arguments.
 */
inline void simulateCall(GPRState *ctx, rword returnAddress, const std::vector<rword>& args = {}) {
    qbdi_simulateCallA(ctx, returnAddress, args.size(), args.data());
}

/*! Simulate a call by modifying the stack and registers accordingly (stdarg version).
 *
 *  @param[in] ctx           GPRState where the simulated call will be setup. The state needs to 
 *                           point to a valid stack for example setup with allocateVirtualStack().
 *  @param[in] returnAddress Return address of the call to simulate.
 *  @param[in] argNum        The number of arguments in the va_list object.
 *  @param[in] ap            An stdarg va_list object.
 */
inline void simulateCallV(GPRState *ctx, rword returnAddress, uint32_t argNum, va_list ap) {
    qbdi_simulateCallV(ctx, returnAddress, argNum, ap);
}

/*! Simulate a call by modifying the stack and registers accordingly (C array version).
 *
 *  @param[in] ctx           GPRState where the simulated call will be setup. The state needs to 
 *                           point to a valid stack for example setup with allocateVirtualStack().
 *  @param[in] returnAddress Return address of the call to simulate.
 *  @param[in] argNum        The number of arguments in the array args.
 *  @param[in] args          An array or arguments.
 */
inline void simulateCallA(GPRState *ctx, rword returnAddress, uint32_t argNum, const rword* args) {
    qbdi_simulateCallA(ctx, returnAddress, argNum, args);
}


}
}
#endif

#endif // _MEMORY_H_
