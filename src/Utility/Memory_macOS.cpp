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

#include <set>

#include <mach/mach.h>
#include <mach-o/loader.h>
#include <mach-o/dyld.h>
#include <mach-o/dyld_images.h>
#include <mach-o/getsect.h>


namespace QBDI {

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

mach_vm_address_t getDyldAllImageInfoAddr(task_t task, mach_vm_size_t* all_image_info_size) {
    task_dyld_info_data_t dyld_info;
    mach_msg_type_number_t count = TASK_DYLD_INFO_COUNT;
    kern_return_t kr = task_info(task, TASK_DYLD_INFO,
                                 reinterpret_cast<task_info_t>(&dyld_info),
                                 &count);
    if (kr != KERN_SUCCESS) {
        return 0;
    }
    if (all_image_info_size) {
        *all_image_info_size = dyld_info.all_image_info_size;
    }
    return dyld_info.all_image_info_addr;
}


bool getDyldAllImageInfo(task_t task, struct dyld_all_image_infos* all_image_infos) {
    if (!all_image_infos) {
        return false;
    }
    mach_vm_size_t all_image_info_size;
    mach_vm_address_t all_image_info_addr = getDyldAllImageInfoAddr(task, &all_image_info_size);
    vm_size_t size = sizeof(struct dyld_all_image_infos);
    memset(all_image_infos, 0, size);
    kern_return_t kr = vm_read_overwrite(task, all_image_info_addr, size, (vm_address_t) all_image_infos, &size);
    if (kr != KERN_SUCCESS) {
        return false;
    }
    return true;
}


struct dyld_image_info *getImageInfo(task_t task, struct dyld_all_image_infos& all_image_infos, uint32_t i) {
    vm_size_t size = sizeof(struct dyld_image_info);
    struct dyld_image_info* image_info = (struct dyld_image_info*) calloc(1, size);
    // If are asking for more than image count, let's return dyld
    if (all_image_infos.infoArrayCount <= i) {
        image_info->imageLoadAddress = all_image_infos.dyldImageLoadAddress;
        if (all_image_infos.version >= 15) {
            image_info->imageFilePath = all_image_infos.dyldPath;
        }
        image_info->imageFileModDate = -1;
    } else {
        kern_return_t kr = vm_read_overwrite(task, (vm_address_t) &(all_image_infos.infoArray[i]), size, (vm_address_t) image_info, &size);
        if (kr != KERN_SUCCESS) {
            return NULL;
        }
    }
    return image_info;
}


struct STRUCT_HEADER *getImageHeader(task_t task, const struct dyld_image_info* image_info) {
    struct STRUCT_HEADER mh;
    vm_size_t size = sizeof(struct STRUCT_HEADER);
    // read macho header first bytes
    kern_return_t kr = vm_read_overwrite(task, (vm_address_t)image_info->imageLoadAddress, size, (vm_address_t) &mh, &size);
    if ((kr != KERN_SUCCESS) || (mh.magic != MAGIC_HEADER)) {
        return NULL;
    }
    // read whole macho header
    size = mh.sizeofcmds + sizeof(struct STRUCT_HEADER);
    struct STRUCT_HEADER* header = (struct STRUCT_HEADER*) malloc(size);
    kr = vm_read_overwrite(task, (vm_address_t)image_info->imageLoadAddress, size, (vm_address_t) header, &size);
    if (kr != KERN_SUCCESS) {
        free(header);
        return NULL;
    }
    return header;
}


char *getImagePath(task_t task, const struct dyld_image_info* image_info) {
    if (image_info->imageFilePath == nullptr) {
        // see getImageInfo
        if (image_info->imageFileModDate == -1) {
            return strdup("/usr/lib/dyld");
        }
        return NULL;
    }
    vm_size_t size = PATH_MAX;
    char* imagePath = (char*) malloc(size);
    kern_return_t kr = vm_read_overwrite(task, (vm_address_t)image_info->imageFilePath, size, (vm_address_t) imagePath, &size);
    if (kr != KERN_SUCCESS) {
        free(imagePath);
        return NULL;
    }
    char *ret = strdup(imagePath);
    free(imagePath);
    return ret;
}


uintptr_t getImageSlideWithHeader(const struct dyld_image_info* image_info, const struct STRUCT_HEADER* mh) {
    struct load_command        *lc = NULL;
    struct STRUCT_SEG          *seg = NULL;

    for (   lc = (struct load_command *)((uintptr_t)mh + sizeof(struct STRUCT_HEADER));
            (uintptr_t)lc < (uintptr_t)mh + (uintptr_t)mh->sizeofcmds;
            lc = (struct load_command *)((uintptr_t)lc + (uintptr_t)lc->cmdsize)   ) {
        if (lc->cmd == MAGIC_SEG) {
            seg = (struct STRUCT_SEG *)lc;
            if ( strcmp(seg->segname, "__TEXT") == 0 )
                return (char*)(image_info->imageLoadAddress) - (char*)(seg->vmaddr);
        }
    }
    return 0;
}


std::vector<MemoryMap> getCurrentProcessMaps() {
    return getRemoteProcessMaps(getpid());
}

std::vector<MemoryMap> getRemoteProcessMaps(QBDI::rword pid) {
    uint32_t                    icnt = 0;
    struct STRUCT_HEADER       *mh = NULL;
    char                       *path = NULL;
    char                       *name = NULL;
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
    struct dyld_all_image_infos all_image_infos;
    bool res = getDyldAllImageInfo(task, &all_image_infos);
    // add +1 in order to include dyld (see getImageInfo)
    icnt = res ? all_image_infos.infoArrayCount + 1 : 0;

    for (uint32_t i = 0; i < icnt; i++) {
        struct dyld_image_info* image_info = getImageInfo(task, all_image_infos, i);
        if (image_info) {
            mh = getImageHeader(task, image_info);
            if (mh) {
                path = getImagePath(task, image_info);
                if (path) {
                    slide = getImageSlideWithHeader(image_info, mh);

                    // dirty basename, but thead safe
                    name = strrchr(path, '/');
                    if (name++) {
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
                    free(path);
                }
                free(mh);
            }
            free(image_info);
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

}
