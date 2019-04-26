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

    qbdi_MemoryMap *maps = qbdi_getCurrentProcessMaps(&size);
    for(i = 0; i < size; i++) {
        printf("%s (%c%c%c) ", maps[i].name,
                maps[i].permission & QBDI_PF_READ ? 'r' : '-',
                maps[i].permission & QBDI_PF_WRITE ? 'w' : '-',
                maps[i].permission & QBDI_PF_EXEC ? 'x' : '-');
        printf("(%#" PRIRWORD ", %#" PRIRWORD ")\n", maps[i].start, maps[i].end);
    }
    qbdi_freeMemoryMapArray(maps, size);

    return 0;
}
