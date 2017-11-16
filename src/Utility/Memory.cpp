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
#include "llvm/Support/Process.h"

#include "Utility/LogSys.h"
#include "Memory.h"

#if defined(QBDI_OS_DARWIN)
#include <set>

#include <mach/mach.h>
#include <mach-o/loader.h>
#include <mach-o/dyld.h>
#include <mach-o/getsect.h>
#elif defined(QBDI_OS_WIN)
#include <Windows.h>
#include <Psapi.h>
#ifdef UNICODE
#include <locale>
#include <codecvt>
#endif
#endif

#define FRAME_LENGTH       16


namespace QBDI {

#if defined(QBDI_OS_LINUX) || defined(QBDI_OS_ANDROID)
std::vector<MemoryMap> getCurrentProcessMaps() {
    return getRemoteProcessMaps(getpid());
}

std::vector<MemoryMap> getRemoteProcessMaps(QBDI::rword pid) {
    static int BUFFER_SIZE = 256;
    char* line = new char[BUFFER_SIZE];
    FILE* mapfile = nullptr;
    std::vector<MemoryMap> maps;

    snprintf(line, BUFFER_SIZE, "/proc/%llu/maps", (unsigned long long) pid);
    mapfile = fopen(line, "r");
    LogDebug("getRemoteProcessMaps", "Querying memory maps from %s", line);
    RequireAction("getRemoteProcessMaps", mapfile != nullptr, delete[] line; return maps);

    // Process a memory map line in the form of
    // 00400000-0063c000 r-xp 00000000 fe:01 675628    /usr/bin/vim
    while((line = fgets(line, BUFFER_SIZE, mapfile)) != nullptr) {
        char* ptr = nullptr;
        MemoryMap m;

        // Remove \n
        if((ptr = strchr(line, '\n')) != nullptr) {
            *ptr = '\0';
        }
        ptr = line;
        LogDebug("getRemoteProcessMaps", "Parsing line: %s", line);

        // Read range
        m.range.start = strtoul(ptr, &ptr, 16);
        ptr++; // '-'
        m.range.end = strtoul(ptr, &ptr, 16);

        // skip the spaces
        while(isspace(*ptr)) ptr++;

        // Read the permission
        m.permission = QBDI::PF_NONE;
        if(*ptr == 'r') m.permission |= QBDI::PF_READ;
        ptr++;
        if(*ptr == 'w') m.permission |= QBDI::PF_WRITE;
        ptr++;
        if(*ptr == 'x') m.permission |= QBDI::PF_EXEC;
        ptr++;
        ptr++; // skip the protected

        // skip the spaces
        while(isspace(*ptr)) ptr++;

        // Discard the file offset
        strtoul(ptr, &ptr, 16);

        // skip the spaces
        while(isspace(*ptr)) ptr++;

        // Discard the device id
        strtoul(ptr, &ptr, 16);
        ptr++; // ':'
        strtoul(ptr, &ptr, 16);

        // skip the spaces
        while(isspace(*ptr)) ptr++;

        // Discard the inode
        strtoul(ptr, &ptr, 10);

        // skip the spaces
        while(isspace(*ptr)) ptr++;

        // Get the file name
        if((ptr = strrchr(ptr, '/')) != nullptr) {
            m.name = std::string(ptr + 1);
        }
        else {
            m.name = std::string("");
        }

        LogCallback(LogPriority::DEBUG, "getRemoteProcessMaps", [&] (FILE *out) -> void {
            fprintf(out, "Read new map [%" PRIRWORD ", %" PRIRWORD "] %s ", m.range.start, m.range.end, m.name.c_str());
            if(m.permission & QBDI::PF_READ)  fprintf(out, "r");
            if(m.permission & QBDI::PF_WRITE) fprintf(out, "w");
            if(m.permission & QBDI::PF_EXEC)  fprintf(out, "x");
        });
        maps.push_back(m);
    }
    fclose(mapfile);
    delete[] line;
    return maps;
}

#elif defined(QBDI_OS_DARWIN)

std::vector<MemoryMap> getCurrentProcessMaps() {
    return getRemoteProcessMaps(getpid());
}

std::vector<MemoryMap> getRemoteProcessMaps(QBDI::rword pid) {
#ifdef QBDI_BITS_64
#define STRUCT_HEADER mach_header_64
#define STRUCT_SEG segment_command_64
#define MAGIC_HEADER MH_MAGIC_64
#define MAGIC_SEG LC_SEGMENT_64
#else
#define STRUCT_HEADER mach_header
#define STRUCT_SEG segment_command
#define MAGIC_HEADER MH_MAGIC
#define MAGIC_SEG LC_SEGMENT
#endif
    uint32_t                    icnt = 0;
    const struct STRUCT_HEADER *mh = NULL;
    const char                 *path = NULL;
    const char                 *name = NULL;
    uintptr_t                   slide = 0;
    struct load_command        *lc = NULL;
    struct STRUCT_SEG          *seg = NULL;


    std::vector<MemoryMap> memMaps;
    std::vector<MemoryMap> modMaps;
    std::vector<MemoryMap> omaps;

    vm_region_submap_short_info_data_64_t basic_info;
    vm_address_t addr = 0, next = addr;
    vm_size_t size = 0;
    mach_msg_type_number_t count = VM_REGION_SUBMAP_SHORT_INFO_COUNT_64;
    uint32_t depth = 1;
    task_t task = 0;
    kern_return_t kr;

    kr = task_for_pid(mach_task_self(), pid, &task);
    RequireAction("getRemoteProcessMaps", kr == KERN_SUCCESS, return memMaps);

    // Create a memory map
    while(1) {
        MemoryMap m;

        kr = vm_region_recurse_64(task, &next, &size, &depth, (vm_region_recurse_info_t) &basic_info, &count);
        if (kr != KERN_SUCCESS) {
            break;
        }

        if(basic_info.is_submap) {
            depth++;
            continue;
        }

        addr = next;
        next += size;

        // add a new memory map
        m.range.start = (rword) addr;
        m.range.end = (rword) next;
        m.name = "";
        m.permission = static_cast<Permission>(basic_info.protection);
        memMaps.push_back(m);
    }

    // Create a map of loaded images segments
    icnt = _dyld_image_count();
    for (int i = 0; i < icnt; i++) {
        mh = (const struct STRUCT_HEADER *) _dyld_get_image_header(i);
        if (!mh || (mh->magic != MAGIC_HEADER)) {
            continue;
        }
        path = _dyld_get_image_name(i);
        slide = _dyld_get_image_vmaddr_slide(i);

        // dirty basename, but thead safe
        name = strrchr(path, '/');
        if (!(name++)) {
            continue;
        }

        // Scan all segments
        for (   lc = (struct load_command *)((uintptr_t)mh + sizeof(struct STRUCT_HEADER));
                (uintptr_t)lc < (uintptr_t)mh + (uintptr_t)mh->sizeofcmds;
                lc = (struct load_command *)((uintptr_t)lc + (uintptr_t)lc->cmdsize)   ) {
            if (lc->cmd == MAGIC_SEG) {
                seg = (struct STRUCT_SEG *)lc;
                // Skip __ZERO segment
                if (!seg->fileoff && !seg->filesize) {
                    continue;
                }

                // Create a map entry
                MemoryMap m;
                m.range.start = seg->vmaddr + slide;
                m.range.end = m.range.start + seg->vmsize;
                m.name = std::string(name);

                modMaps.push_back(m);
            }
        }
    }

    // Merge our two maps into a consitent view of the memory
    std::set<rword> inserted;
    for (const MemoryMap& mem: memMaps) {
        // create a range set of current memory range
        RangeSet<rword> rs;
        rs.add(mem.range);

        // try to map modules to current memory range
        for (const MemoryMap& mod: modMaps) {
            if (mem.range.overlaps(mod.range)) {
                // skip if already inserted (due to previous overlap)
                if (inserted.count(mod.range.start))
                    continue;
                // add new entry in map
                MemoryMap m;
                m.range = mod.range;
                m.permission = mem.permission;
                // do not add name to the shared __LINKEDIT
                if (m.permission == PF_READ && !(mod.range == mem.range)) {
                    m.name = "";
                } else {
                    m.name = mod.name;
                }
                omaps.push_back(m);
                // mark range as inserted
                inserted.insert(m.range.start);
                // remove module range from memory range
                rs.remove(mod.range);
            }
        }
        // add unmatched ranges in memory map
        for (const Range<rword>& r: rs.getRanges()) {
            MemoryMap m;
            m.range = r;
            m.name = "";
            m.permission = mem.permission;
            omaps.push_back(m);
        }
    }
    // sort the probably unordered map
    std::sort(omaps.begin(), omaps.end(),
            [](const MemoryMap& a, const MemoryMap& b) -> bool {
                return a.range.start < b.range.start;
            });

    return omaps;
}

#elif defined(QBDI_OS_WIN)

#define PROT_ISREAD(PROT)   ((PROT) & 0xEE)
#define PROT_ISWRITE(PROT)  ((PROT) & 0xCC)
#define PROT_ISEXEC(PROT)   ((PROT) & 0xF0)

std::vector<MemoryMap> getCurrentProcessMaps() {
    return getRemoteProcessMaps(GetCurrentProcessId());
}

std::vector<MemoryMap> getRemoteProcessMaps(QBDI::rword pid) {
    std::vector<MemoryMap> maps;
    MEMORY_BASIC_INFORMATION info;
    HMODULE mod;
    rword addr = 0;
    rword next = addr;
    SIZE_T size = 0;
    TCHAR path[MAX_PATH];

    HANDLE self = OpenProcess(PROCESS_QUERY_INFORMATION |
                              PROCESS_VM_OPERATION |
                              PROCESS_VM_READ,
                              FALSE,
                              pid);
    if (self == NULL) {
        return maps;
    }

    while (VirtualQueryEx(self, (LPVOID) next, &info, sizeof(info)) != 0) {
        // extract page info
        addr = (rword) info.BaseAddress;
        size = info.RegionSize;
        next = addr + size;

        // skip reserved / free pages
        if (info.State != MEM_COMMIT) {
            continue;
        }

        // create new memory map entry
        MemoryMap m;
        m.range.start = addr;
        m.range.end = next;
        m.permission = QBDI::PF_NONE;
        m.permission |= PROT_ISREAD(info.Protect) ? QBDI::PF_READ : QBDI::PF_NONE;
        m.permission |= PROT_ISWRITE(info.Protect) ? QBDI::PF_WRITE : QBDI::PF_NONE;
        m.permission |= PROT_ISEXEC(info.Protect) ? QBDI::PF_EXEC : QBDI::PF_NONE;

        // try to get current module name
        // TODO: this is inefficient, but it was easy to implement ():)
        DWORD ret = 0;
        if (info.Type == MEM_IMAGE) {
            ret = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR) addr, &mod);
        }
        if (ret != 0 && mod != NULL) {
            ret = GetModuleBaseName(self, mod, path, _countof(path));
            FreeLibrary(mod);
        }
        if (ret != 0) {
#ifdef UNICODE
            std::wstring wstr(path);
            std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
            m.name = conv.to_bytes(wstr);
#else
            m.name = std::string(path);
#endif
        } else {
            // fallback to empty name
            m.name = "";
        }
        maps.push_back(m);
    }
    CloseHandle(self);
    return maps;
}

#endif

std::vector<std::string> getModuleNames() {
    std::vector<std::string> modules;

    for(const MemoryMap& m : getCurrentProcessMaps()) {
        if(m.name != "") {
            bool exist = false;

            for(const std::string& s : modules) {
                if(s == m.name) {
                    exist = true;
                }
            }

            if(!exist) {
                modules.push_back(m.name);
            }
        }
    }

    return modules;
}


char** qbdi_getModuleNames(size_t* size) {
    std::vector<std::string> modules = getModuleNames();
    *size = modules.size();
    if(*size == 0) {
        return NULL;
    }
    char** names = (char**) malloc(modules.size() * sizeof(char**));
    for(size_t i = 0; i < modules.size(); i++) {
        names[i] = (char*) malloc((modules[i].length() + 1) * sizeof(char));
        strncpy(names[i], modules[i].c_str(), modules[i].length() + 1);
    }
    return names;
}


bool qbdi_allocateVirtualStack(GPRState *ctx, uint32_t stackSize, uint8_t **stack) {
    (*stack) = (uint8_t*) alignedAlloc(stackSize, 16);
    if(*stack == nullptr) {
        return false;
    }

    QBDI_GPR_SET(ctx, REG_SP, (QBDI::rword) (*stack) + stackSize);
    QBDI_GPR_SET(ctx, REG_BP, QBDI_GPR_GET(ctx, REG_SP));

    return true;
}

void qbdi_simulateCallA(GPRState *ctx, rword returnAddress, uint32_t argNum, const rword* args) {
    uint32_t i = 0;
    uint32_t argsoff = 0;
    uint32_t limit = FRAME_LENGTH;

    // Allocate arguments frame
    QBDI_GPR_SET(ctx, REG_SP, QBDI_GPR_GET(ctx, REG_SP) - FRAME_LENGTH * sizeof(rword));

    // Handle the return address
#if defined(QBDI_ARCH_X86_64)
    ctx->rsp -= sizeof(rword);
    *((rword*) ctx->rsp) = returnAddress;
    argsoff++;
#elif defined(QBDI_ARCH_ARM)
    ctx->lr = returnAddress;
#endif

#define UNSTACK_ARG(REG) if (i < argNum) { ctx->REG = args[i++]; }
#if defined(QBDI_ARCH_X86_64)
 #if defined(QBDI_OS_WIN)
    // Shadow space
    argsoff += 4;
    // Register args
    UNSTACK_ARG(rcx);
    UNSTACK_ARG(rdx);
    UNSTACK_ARG(r8);
    UNSTACK_ARG(r9);
 #else
    // Register args
    UNSTACK_ARG(rdi);
    UNSTACK_ARG(rsi);
    UNSTACK_ARG(rdx);
    UNSTACK_ARG(rcx);
    UNSTACK_ARG(r8);
    UNSTACK_ARG(r9);
 #endif // OS
#elif defined(QBDI_ARCH_ARM)
    UNSTACK_ARG(r0);
    UNSTACK_ARG(r1);
    UNSTACK_ARG(r2);
    UNSTACK_ARG(r3);
#endif // ARCH
#undef UNSTACK_ARG
    limit -= argsoff;

    // Push remaining args on the stack
    rword* frame = (rword*) QBDI_GPR_GET(ctx, REG_SP);
    for (uint32_t j = 0; (i + j) < argNum && j < limit; j++) {
        frame[argsoff + j] = args[i + j];
    }
}

void qbdi_simulateCallV(GPRState *ctx, rword returnAddress, uint32_t argNum, va_list ap) {
    rword* args = new rword[argNum];
    for(uint32_t i = 0; i < argNum; i++) {
        args[i] = va_arg(ap, rword);
    }
    qbdi_simulateCallA(ctx, returnAddress, argNum, args);
    delete[] args;
}

void qbdi_simulateCall(GPRState *ctx, rword returnAddress, uint32_t argNum, ...) {
    va_list  ap;
    // Handle the arguments
    va_start(ap, argNum);
    qbdi_simulateCallV(ctx, returnAddress, argNum, ap);
    va_end(ap);
}

void* qbdi_alignedAlloc(size_t size, size_t align) {
    void* allocated = nullptr;
    // Alignment needs to be a power of 2
    if ((align == 0) || ((align & (align - 1)) != 0)) {
        return nullptr;
    }
#if defined(QBDI_OS_LINUX) || defined(QBDI_OS_ANDROID) || defined(QBDI_OS_DARWIN)
    int ret = posix_memalign(&allocated, align, size);
    if (ret != 0) {
        return nullptr;
    }
#elif defined(QBDI_OS_WIN)
    allocated = _aligned_malloc(size, align);
#endif
    return allocated;
}

void qbdi_alignedFree(void* ptr) {
#if defined(QBDI_OS_LINUX) || defined(QBDI_OS_ANDROID) || defined(QBDI_OS_DARWIN)
    free(ptr);
#elif defined(QBDI_OS_WIN)
    _aligned_free(ptr);
#endif
}

}
