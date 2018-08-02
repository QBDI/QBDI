#include <stdlib.h>
#include <stdio.h>

#include <QBDI.h>

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

    return 0;
}
