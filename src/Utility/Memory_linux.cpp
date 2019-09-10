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
#include "Memory.hpp"
#include "Memory.h"


namespace QBDI {

std::vector<MemoryMap> getCurrentProcessMaps(bool full_path) {
    return getRemoteProcessMaps(getpid(), full_path);
}

std::vector<MemoryMap> getRemoteProcessMaps(QBDI::rword pid, bool full_path) {
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
        if (full_path) {
            m.name = ptr;
        } else {
            if((ptr = strrchr(ptr, '/')) != nullptr) {
                m.name = ptr + 1;
            } else {
                m.name.clear();
            }
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

}
