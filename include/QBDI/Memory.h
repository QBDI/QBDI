/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2021 Quarkslab
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
#ifndef QBDI_MEMORY_H_
#define QBDI_MEMORY_H_

#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#include "QBDI/Platform.h"
#include "QBDI/State.h"

#ifdef __cplusplus
namespace QBDI {
extern "C" {
#endif

/*! Memory access rights.
 */
typedef enum {
    QBDI_PF_NONE = 0,   /*!< No access */
    QBDI_PF_READ = 1,   /*!< Read access */
    QBDI_PF_WRITE = 2,  /*!< Write access */
    QBDI_PF_EXEC = 4    /*!< Execution access */
} qbdi_Permission;


/*! Map of a memory area (region).
 */
typedef struct {
    rword            start;          /*!< Range start value. */
    rword            end;            /*!< Range end value (always excluded). */
    qbdi_Permission  permission;     /*!< Region access rights (PF_READ, PF_WRITE, PF_EXEC). */
    char*            name;           /*!< Region name or path (useful when a region is mapping a module). */
} qbdi_MemoryMap;

/*! Get a list of all the memory maps (regions) of a process.
 *
 * @param[in]  pid  The identifier of the process.
 * @param[in]  full_path  Return the full path of the module in name field
 * @param[out] size Will be set to the number of strings in the returned array.
 *
 * @return  An array of MemoryMap object.
 */
QBDI_EXPORT qbdi_MemoryMap* qbdi_getRemoteProcessMaps(rword pid, bool full_path, size_t* size);

/*! Get a list of all the memory maps (regions) of the current process.
 *
 * @param[in]  full_path  Return the full path of the module in name field
 * @param[out] size Will be set to the number of strings in the returned array.
 *
 * @return  An array of MemoryMap object.
 */
QBDI_EXPORT qbdi_MemoryMap* qbdi_getCurrentProcessMaps(bool full_path, size_t* size);

/*! Free an array of memory maps objects.
 *
 * @param[in] arr  An array of MemoryMap object.
 * @param[in] size Number of elements in the array.
 */
QBDI_EXPORT void qbdi_freeMemoryMapArray(qbdi_MemoryMap* arr, size_t size);

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
}
}
#endif

#endif // QBDI_MEMORY_H_
