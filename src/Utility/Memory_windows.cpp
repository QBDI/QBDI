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
#include "llvm/Support/Process.h"

#include "Utility/LogSys.h"
#include "Memory.hpp"
#include "Memory.h"

#include <Windows.h>
#include <Psapi.h>
#ifdef UNICODE
#include <locale>
#include <codecvt>
#endif


namespace QBDI {

#define PROT_ISREAD(PROT)   ((PROT) & 0xEE)
#define PROT_ISWRITE(PROT)  ((PROT) & 0xCC)
#define PROT_ISEXEC(PROT)   ((PROT) & 0xF0)

std::vector<MemoryMap> getCurrentProcessMaps(bool full_path) {
    return getRemoteProcessMaps(GetCurrentProcessId(), full_path);
}

std::vector<MemoryMap> getRemoteProcessMaps(QBDI::rword pid, bool full_path) {
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
        m.range.setStart(addr);
        m.range.setEnd(next);
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
            if (full_path) {
                ret = GetModuleFileNameEx(self, mod, path, _countof(path));
            } else {
                ret = GetModuleBaseName(self, mod, path, _countof(path));
            }
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
            m.name.clear();
        }
        LogCallback(LogPriority::DEBUG, "getRemoteProcessMaps", [&] (FILE *out) -> void {
            fprintf(out, "Read new map [%" PRIRWORD ", %" PRIRWORD "] %s ", m.range.start(), m.range.end(), m.name.c_str());
            if(m.permission & QBDI::PF_READ)  fprintf(out, "r");
            if(m.permission & QBDI::PF_WRITE) fprintf(out, "w");
            if(m.permission & QBDI::PF_EXEC)  fprintf(out, "x");
        });
        maps.push_back(m);
    }
    CloseHandle(self);
    return maps;
}

}
