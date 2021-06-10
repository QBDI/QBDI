
#include <stddef.h>
#include <windows.h>

#include "QBDI/Platform.h"

QBDI_EXPORT BOOLEAN WINAPI DllMain(IN HINSTANCE hDllHandle, IN DWORD nReason, IN LPVOID Reserved) {
    switch (nReason) {
        case DLL_PROCESS_ATTACH:
            //  For optimization.
            DisableThreadLibraryCalls(hDllHandle);
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}


