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

#include <link.h>
#include <unistd.h>
#include <list>


namespace QBDI {

static int dl_iterate_phdr_cbk(struct dl_phdr_info *info, size_t size, void *data) {
    static const std::vector<Elf32_Word> type_order = {PT_LOAD, PT_GNU_EH_FRAME, PT_GNU_RELRO};
    static const unsigned long pagesize = sysconf(_SC_PAGESIZE);
    std::vector<MemoryMap> *maps = static_cast<std::vector<MemoryMap>*>(data);
    std::list<MemoryMap> locallist;

    printf("name=%s 0x%lx (%d segments)\n", info->dlpi_name, info->dlpi_addr,
            info->dlpi_phnum);

    const char* memory_name = strrchr(info->dlpi_name, '/');
    if (memory_name == nullptr) {
        memory_name = info->dlpi_name;
    } else {
        memory_name++;
    }

    for (Elf32_Word type : type_order) {
        for (int j = 0; j < info->dlpi_phnum; j++) {

            if (type == PT_LOAD)
                printf("\t\t header %2d: address=%10p\tvaddr=0x%08lx\tfilesize=0x%08lx\tmemsize=0x%08lx\tflags=0x%x\talign=%ld\n", j,
                    (void *) (info->dlpi_addr + info->dlpi_phdr[j].p_vaddr),
                    info->dlpi_phdr[j].p_vaddr,
                    info->dlpi_phdr[j].p_filesz,
                    info->dlpi_phdr[j].p_memsz,
                    info->dlpi_phdr[j].p_flags,
                    info->dlpi_phdr[j].p_align);

            if (info->dlpi_phdr[j].p_type != type || info->dlpi_phdr[j].p_memsz == 0)
                continue;

            rword begin_addr = info->dlpi_addr + info->dlpi_phdr[j].p_vaddr;
            rword end_addr = begin_addr + info->dlpi_phdr[j].p_memsz;

            // align
            long lpagesize = (pagesize > info->dlpi_phdr[j].p_align) ? pagesize : info->dlpi_phdr[j].p_align;
            begin_addr = begin_addr & ~(lpagesize - 1);
            end_addr = ((end_addr - 1) & ~(lpagesize - 1)) + lpagesize;

            // perm
            Permission p = QBDI::PF_NONE;
            if(info->dlpi_phdr[j].p_flags & PF_R) p |= QBDI::PF_READ;
            if(info->dlpi_phdr[j].p_flags & PF_W) p |= QBDI::PF_WRITE;
            if(info->dlpi_phdr[j].p_flags & PF_X) p |= QBDI::PF_EXEC;

            // add section in current sorted list, with merge or split
            for (std::list<MemoryMap>::iterator iter = locallist.begin(); iter != locallist.end(); iter++) {
                if (end_addr <= begin_addr)
                    break;
                if (iter->end <= begin_addr)
                    continue;
                if (end_addr < iter->start || (p != iter->permission && end_addr == iter->start)) {
                    // add full range before this element
                    locallist.insert(iter, MemoryMap(begin_addr, end_addr, p, memory_name));
                    begin_addr = end_addr;
                    break;
                }
                if (p == iter->permission) {
                    if (begin_addr < iter->start)
                        iter->start = begin_addr;
                    if (iter->end < end_addr) {
                        begin_addr = iter->end;
                        continue;
                    } else {
                        begin_addr = end_addr;
                        break;
                    }
                } else {
                    if (begin_addr < iter->start) {
                        locallist.insert(iter, MemoryMap(begin_addr, iter->start, p, memory_name));
                        begin_addr = iter->start;
                    } else if (iter->start < begin_addr) {
                        locallist.insert(iter, MemoryMap(iter->start, begin_addr, iter->permission, memory_name));
                        iter->start = begin_addr;
                    }
                    if (iter->end < end_addr) {
                        iter->permission = p;
                        begin_addr = iter->end;
                        continue;
                    } else if (iter->end == end_addr) {
                        iter->permission = p;
                        begin_addr = end_addr;
                        break;
                    } else /* end_addr < iter->end */ {
                        locallist.insert(iter, MemoryMap(begin_addr, end_addr, p, memory_name));
                        iter->start = end_addr;
                        begin_addr = end_addr;
                        break;
                    }
                }
            }
            if (begin_addr < end_addr)
                locallist.push_back(MemoryMap(begin_addr, end_addr, p, memory_name));
        }
    }
    // merge

    for (std::list<MemoryMap>::iterator iter = locallist.begin(); iter != locallist.end(); ) {
        MemoryMap& previous = *iter;
        iter++;
        while (iter != locallist.end() &&
                previous.permission == iter->permission &&
                previous.end == iter->start) {
            previous.end = iter->end;
            iter = locallist.erase(iter);
        }
    }

    // add to maps
    maps->insert(maps->end(), locallist.begin(), locallist.end());
    return 0;
}

std::vector<MemoryMap> getCurrentProcessMaps2() {
    std::vector<MemoryMap> maps;

    dl_iterate_phdr(dl_iterate_phdr_cbk, &maps);
    printf("res:\n");
    for (MemoryMap &mem : maps) {
        printf("\t\tstart : 0x%08lx\tend : 0x%08lx\tperm : %c%c%c\tname : %s\n",
                mem.start, mem.end,
                QBDI::PF_READ & mem.permission ? 'r' : '-',
                QBDI::PF_WRITE & mem.permission ? 'w' : '-',
                QBDI::PF_EXEC & mem.permission ? 'x' : '-',
                mem.name);
    }


    return maps;
}

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
            m.name = ptr + 1;
        } else {
            m.name.clear();
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

