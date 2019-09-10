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
#ifndef QBDI_MEMORY_HPP_
#define QBDI_MEMORY_HPP_

#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#include "Platform.h"
#include "State.h"

#include <string>
#include <vector>
#include "Range.h"
#include "Bitmask.h"


namespace QBDI {

/*! Memory access rights.
 */
typedef enum {
    PF_NONE = 0,   /*!< No access */
    PF_READ = 1,   /*!< Read access */
    PF_WRITE = 2,  /*!< Write access */
    PF_EXEC = 4    /*!< Execution access */
} Permission;

_QBDI_ENABLE_BITMASK_OPERATORS(Permission)

/*! Map of a memory area (region).
 */
struct MemoryMap {

    Range<rword> range;         /*!< A range of memory (region), delimited between a start and an (excluded) end address. */
    Permission  permission;     /*!< Region access rights (PF_READ, PF_WRITE, PF_EXEC). */
    std::string name;           /*!< Region name or path (useful when a region is mapping a module). */

    /* Construct a new (empty) MemoryMap.
     */
    MemoryMap() : range(0, 0), permission(QBDI::PF_NONE) {};

    /*! Construct a new MemoryMap (given some properties).
     *
     * @param[in] start        Range start value.
     * @param[in] end          Range end value (always excluded).
     * @param[in] permission   Region access rights (PF_READ, PF_WRITE, PF_EXEC).
     * @param[in] name         Region name (useful when a region is mapping a module).
     */
    MemoryMap(rword start, rword end, Permission permission, std::string name) :
        range(start, end), permission(permission), name(name) {}

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
 * @param[in] full_path  Return the full path of the module in name field
 *
 * @return  A vector of MemoryMap object.
 */
QBDI_EXPORT std::vector<MemoryMap> getRemoteProcessMaps(QBDI::rword pid, bool full_path=false);

/*! Get a list of all the memory maps (regions) of the current process.
 *
 * @param[in] full_path  Return the full path of the module in name field
 * @return  A vector of MemoryMap object.
 */
QBDI_EXPORT std::vector<MemoryMap> getCurrentProcessMaps(bool full_path=false);

/*! Get a list of all the module names loaded in the process memory.
 *
 * @return  A vector of string of module names.
 */
QBDI_EXPORT std::vector<std::string> getModuleNames();





/*! Allocate a block of memory of a specified sized with an aligned base address.
 *
 * @param[in] size  Allocation size in bytes.
 * @param[in] align Base address alignement in bytes.
 *
 * @return  Pointer to the allocated memory or NULL in case an error was encountered.
 *
*/
QBDI_EXPORT void* alignedAlloc(size_t size, size_t align);

/*! Free a block of aligned memory allocated with alignedAlloc.
 *
 * @param[in] ptr  Pointer to the allocated memory.
 *
*/
QBDI_EXPORT void alignedFree(void* ptr);

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
QBDI_EXPORT bool allocateVirtualStack(GPRState *ctx, uint32_t stackSize, uint8_t **stack);

/*! Simulate a call by modifying the stack and registers accordingly (std::vector version).
 *
 *  @param[in] ctx           GPRState where the simulated call will be setup. The state needs to
 *                           point to a valid stack for example setup with allocateVirtualStack().
 *  @param[in] returnAddress Return address of the call to simulate.
 *  @param[in] args          A list of arguments.
 */
QBDI_EXPORT void simulateCall(GPRState *ctx, rword returnAddress, const std::vector<rword>& args = {});

/*! Simulate a call by modifying the stack and registers accordingly (stdarg version).
 *
 *  @param[in] ctx           GPRState where the simulated call will be setup. The state needs to
 *                           point to a valid stack for example setup with allocateVirtualStack().
 *  @param[in] returnAddress Return address of the call to simulate.
 *  @param[in] argNum        The number of arguments in the va_list object.
 *  @param[in] ap            An stdarg va_list object.
 */
QBDI_EXPORT void simulateCallV(GPRState *ctx, rword returnAddress, uint32_t argNum, va_list ap);

/*! Simulate a call by modifying the stack and registers accordingly (C array version).
 *
 *  @param[in] ctx           GPRState where the simulated call will be setup. The state needs to
 *                           point to a valid stack for example setup with allocateVirtualStack().
 *  @param[in] returnAddress Return address of the call to simulate.
 *  @param[in] argNum        The number of arguments in the array args.
 *  @param[in] args          An array or arguments.
 */
QBDI_EXPORT void simulateCallA(GPRState *ctx, rword returnAddress, uint32_t argNum, const rword* args);


}

#endif // QBDI_MEMORY_HPP_
