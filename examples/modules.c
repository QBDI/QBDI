#include <stdlib.h>
#include <stdio.h>
#include "Memory.h"

int main(int argc, char** argv) {
    size_t size = 0, i = 0;
    char **modules = qbdi_getModuleNames(&size);

    for(i = 0; i < size; i++) {
        printf("%s\n", modules[i]);
    }

    for(i = 0; i < size; i++) {
        free(modules[i]);
    }
    free(modules);

    MemoryMap *maps = qbdi_getCurrentProcessMaps(&size);
    for(i = 0; i < size; i++) {
        printf("%s (%d) ", maps[i].name, maps[i].permission);
        printf("(%#" PRIRWORD ", %#" PRIRWORD ")\n", maps[i].start, maps[i].end);
    }
    qbdi_freeMemoryMapArray(maps, size);

    return 0;
}
