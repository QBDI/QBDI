/*
 * This file is part of QBDI.
 *
 * Copyright 2019 Quarkslab
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
#include "QBDIPreload.h"
#include <Windows.h>
#include <winnt.h>
#include <tlhelp32.h>

/* Consts */
#define QBDIPRELOAD_SHARED_MEMORY_NAME_FMT  "qbdi_preload_%u"
static const unsigned long INST_INT1 = 0x01CD;          /* Instruction opcode for "INT 1"        */
static const unsigned long INST_INT1_MASK = 0xFFFF;
static const size_t QBDI_RUNTIME_STACK_SIZE = 0x800000; /* QBDI shadow stack size                */
static const long MEM_PAGE_SIZE = 4096;                 /* Default page size on Windows          */
static const unsigned int SH_MEM_SIZE = 4096;           /* Shared memory size (attach mode only) */

/* Trampoline spec (to be called from assembly stub) */
void qbdipreload_trampoline();

/* Globals */
static struct {
    void* va;
    uint64_t orig_bytes;
} g_EntryPointInfo;                     /* Main module EntryPoint (PE from host process)                                 */
static PVOID g_hExceptionHandler;       /* VEH for QBDI preload internals (break on EntryPoint)                          */
static rword g_firstInstructionVA;      /* First instruction that will be executed by QBDI                               */
static rword g_lastInstructionVA;       /* Last instruction that will be executed by QBDI                                */
PVOID g_shadowStackTop = NULL;          /* QBDI shadow stack top pointer(decreasing address)                             */
static GPRState g_EntryPointGPRState;   /* QBDI CPU GPR states when EntryPoint has been reached                          */
static FPRState g_EntryPointFPRState;   /* QBDI CPU FPR states when EntryPoint has been reached                          */
static HANDLE g_hMainThread;            /* Main thread handle (attach mode only)                                         */
static HANDLE g_hShMemMap;              /* Shared memory object between qbdipreload & external binary (attach mode only) */
static LPVOID g_pShMem;                 /* Shared memory base pointer (attach mode only)                                 */
static BOOL g_bIsAttachMode;            /* TRUE if attach mode is activated                                              */

/*
 * Conversion from windows CONTEXT ARCH dependent structure 
 * to QBDI GPR state (Global purpose registers)
 */
void qbdipreload_threadCtxToGPRState(const void* gprCtx, GPRState* gprState) {
    PCONTEXT x64cpu = (PCONTEXT) gprCtx;

    gprState->rax = x64cpu->Rax;
    gprState->rbx = x64cpu->Rbx;
    gprState->rcx = x64cpu->Rcx;
    gprState->rdx = x64cpu->Rdx;
    gprState->rsi = x64cpu->Rsi;
    gprState->rdi = x64cpu->Rdi;
    gprState->rbp = x64cpu->Rbp;
    gprState->rsp = x64cpu->Rsp;
    gprState->r8 = x64cpu->R8;
    gprState->r9 = x64cpu->R9;
    gprState->r10 = x64cpu->R10;
    gprState->r11 = x64cpu->R11;
    gprState->r12 = x64cpu->R12;
    gprState->r13 = x64cpu->R13;
    gprState->r14 = x64cpu->R14;
    gprState->r15 = x64cpu->R15;
    gprState->rip = x64cpu->Rip;
    gprState->eflags = x64cpu->EFlags;
}

/*
 * Conversion from windows CONTEXT ARCH dependent structure 
 * to QBDI FPR state (Floating point registers)
 */
void qbdipreload_floatCtxToFPRState(const void* gprCtx, FPRState* fprState) {
    PCONTEXT x64cpu = (PCONTEXT) gprCtx;

    // FPU STmm(X)
    memcpy(&fprState->stmm0, &x64cpu->FltSave.FloatRegisters[0], sizeof(MMSTReg));
    memcpy(&fprState->stmm1, &x64cpu->FltSave.FloatRegisters[1], sizeof(MMSTReg));
    memcpy(&fprState->stmm2, &x64cpu->FltSave.FloatRegisters[2], sizeof(MMSTReg));
    memcpy(&fprState->stmm3, &x64cpu->FltSave.FloatRegisters[3], sizeof(MMSTReg));
    memcpy(&fprState->stmm4, &x64cpu->FltSave.FloatRegisters[4], sizeof(MMSTReg));
    memcpy(&fprState->stmm5, &x64cpu->FltSave.FloatRegisters[5], sizeof(MMSTReg));
    memcpy(&fprState->stmm6, &x64cpu->FltSave.FloatRegisters[6], sizeof(MMSTReg));
    memcpy(&fprState->stmm7, &x64cpu->FltSave.FloatRegisters[7], sizeof(MMSTReg));

    // XMM(X) registers
    memcpy(&fprState->xmm0, &x64cpu->Xmm0, 16);
    memcpy(&fprState->xmm1, &x64cpu->Xmm1, 16);
    memcpy(&fprState->xmm2, &x64cpu->Xmm2, 16);
    memcpy(&fprState->xmm3, &x64cpu->Xmm3, 16);
    memcpy(&fprState->xmm4, &x64cpu->Xmm4, 16);
    memcpy(&fprState->xmm5, &x64cpu->Xmm5, 16);
    memcpy(&fprState->xmm6, &x64cpu->Xmm6, 16);
    memcpy(&fprState->xmm7, &x64cpu->Xmm7, 16);
    memcpy(&fprState->xmm8, &x64cpu->Xmm8, 16);
    memcpy(&fprState->xmm9, &x64cpu->Xmm9, 16);
    memcpy(&fprState->xmm10, &x64cpu->Xmm10, 16);
    memcpy(&fprState->xmm11, &x64cpu->Xmm11, 16);
    memcpy(&fprState->xmm12, &x64cpu->Xmm12, 16);
    memcpy(&fprState->xmm13, &x64cpu->Xmm13, 16);
    memcpy(&fprState->xmm14, &x64cpu->Xmm14, 16);
    memcpy(&fprState->xmm15, &x64cpu->Xmm15, 16);

    // Others FPU registers
    fprState->rfcw = x64cpu->FltSave.ControlWord;
    fprState->rfsw = x64cpu->FltSave.StatusWord;
    fprState->ftw  = x64cpu->FltSave.TagWord;
    fprState->rsrv1 = x64cpu->FltSave.Reserved1;
    fprState->ip =  x64cpu->FltSave.ErrorOffset;
    fprState->cs =  x64cpu->FltSave.ErrorSelector;
    fprState->rsrv2 = x64cpu->FltSave.Reserved2;
    fprState->dp =  x64cpu->FltSave.DataOffset;
    fprState->ds =  x64cpu->FltSave.DataSelector;
    fprState->rsrv3 = x64cpu->FltSave.Reserved3;
    fprState->mxcsr = x64cpu->FltSave.MxCsr;
    fprState->mxcsrmask =  x64cpu->FltSave.MxCsr_Mask;
 }

/*
 * Write an "int 1" instruction at given address
 * Save previous byte to internal buffer
 * return 0 in case of failure
 */
int setInt1Exception(void* fn_va) {
     DWORD oldmemprot;

    if (!fn_va){
        return 0;
    }

    uintptr_t base = (uintptr_t) fn_va - ((uintptr_t) fn_va % MEM_PAGE_SIZE);
    
    g_EntryPointInfo.va = fn_va;
    if(!VirtualProtect((void*) base, MEM_PAGE_SIZE, PAGE_READWRITE, &oldmemprot))
        return 0;
    g_EntryPointInfo.orig_bytes = *(uint64_t*) fn_va;
    *(uint64_t*) fn_va = INST_INT1 | (g_EntryPointInfo.orig_bytes & (~(uint64_t)INST_INT1_MASK));
    
    return VirtualProtect((void*) base, MEM_PAGE_SIZE, oldmemprot, &oldmemprot) != 0;
}

/*
 * Restore original bytes on a previously installed "int 1" instruction
 * return 0 in case of failure
 */
int unsetInt1Exception() {
	DWORD oldmemprot;
    uintptr_t base = (uintptr_t) g_EntryPointInfo.va - ((uintptr_t) g_EntryPointInfo.va % MEM_PAGE_SIZE);

    if(!VirtualProtect((void*) base, MEM_PAGE_SIZE, PAGE_READWRITE, &oldmemprot))
        return 0;
    
    *(uint64_t*) g_EntryPointInfo.va = g_EntryPointInfo.orig_bytes;
    
    return VirtualProtect((void*) base, MEM_PAGE_SIZE, oldmemprot, &oldmemprot) != 0;
}

/*
 * Remove a previously installed vectored exception handler
 * Return 0 in case of failure
 */
int unsetExceptionHandler(LONG (*exception_filter_fn)(PEXCEPTION_POINTERS)) {
    return RemoveVectoredExceptionHandler(exception_filter_fn);
}

/*
 * Install a vectored exception handler
 * Return 0 in case of failure
 */
int setExceptionHandler(LONG (*exception_filter_fn)(PEXCEPTION_POINTERS)) {
    
    g_hExceptionHandler = AddVectoredExceptionHandler(1, exception_filter_fn);
    return g_hExceptionHandler != NULL;
}

/*
 * Trampoline implementation
 * It removes exception handler, restore entry point bytes and 
 * setup QBDI runtime for host target before calling user callback "on_run"
 * Its is called from separate qbdipreload_trampoline() assembly stub that
 * makes this function load in a arbitraty allocated stack, then QBDI can
 * safely initialize & instrument main target thread
 */
void qbdipreload_trampoline_impl() {
    unsetInt1Exception();
    unsetExceptionHandler(g_hExceptionHandler);

    // On windows only entry point call is catched
    // but not main function
    int status = qbdipreload_on_main(0, NULL);
    
    if(status == QBDIPRELOAD_NOT_HANDLED) {
        VMInstanceRef vm;
        qbdi_initVM(&vm, NULL, NULL);
    
        // Filter some modules to avoid conflicts
        qbdi_removeAllInstrumentedRanges(vm);

        // Set original CPU state
        qbdi_setGPRState(vm, &g_EntryPointGPRState);
        qbdi_setFPRState(vm, &g_EntryPointFPRState);

        // User final callback call as QBDIPreload is ready
        status = qbdipreload_on_run(vm, g_firstInstructionVA, g_lastInstructionVA);
    }

    // Exiting early must be done as qbdipreload_trampoline_impl
    // is executed on fake stack without any caller
    // It will trigger DLL_PROCESS_DETACH event in DLLMain()
    ExitProcess(status);
}

/*
 * Windows QBDI preload specific excetion handler
 * It must be uninstalled once used one time
 * The handler catches the first INT1 exception
 */
LONG WINAPI QbdiPreloadExceptionFilter(PEXCEPTION_POINTERS exc_info) {
    PCONTEXT x64cpu = exc_info->ContextRecord;

    // Sanity check on exception
    if((exc_info->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) &&
        (x64cpu->Rip == (DWORD64) g_EntryPointInfo.va)) {
        // Call user provided callback with x64 cpu state (specific to windows)
        int status = qbdipreload_on_premain((void*) x64cpu, (void*) x64cpu);

        // Convert windows CPU context to QBDIGPR/FPR states
        qbdipreload_threadCtxToGPRState(x64cpu, &g_EntryPointGPRState);
        qbdipreload_floatCtxToFPRState(x64cpu, &g_EntryPointFPRState);

        // First instruction to execute is main module entry point)
        g_firstInstructionVA = QBDI_GPR_GET(&g_EntryPointGPRState, REG_PC);

        // In case if attach mode, it's difficult to guess on which instructoin stopping the instruction
        if (g_bIsAttachMode) {
            g_lastInstructionVA = (rword) 0xFFFFFFFFFFFFFFFF;
        }
        // If start function has been hooked
        // Last instruction to execute is inside windows PE loader
        // (inside BaseThreadInitThunk() who called PE entry point & set RIP on stack) 
        else {
            g_lastInstructionVA =  *((rword*) QBDI_GPR_GET(&g_EntryPointGPRState, REG_SP));
        }

        if (status == QBDIPRELOAD_NOT_HANDLED) {
            // Allocate shadow stack & keep some space at the end for QBDI runtime
            g_shadowStackTop = VirtualAlloc(NULL, QBDI_RUNTIME_STACK_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            g_shadowStackTop = (PVOID) ((uint64_t) g_shadowStackTop + QBDI_RUNTIME_STACK_SIZE - 0x1008);
        }

        // Continue execution on trampoline to make QBDI runtime
        // execute using a separate stack and not instrumented target one
        // RSP can't be set here (system seems to validate pointer authenticity)
        x64cpu->Rip = (uint64_t) qbdipreload_trampoline;
    }

    return EXCEPTION_CONTINUE_EXECUTION;
}

/*
 * Get main module entry point
 */
void* getMainModuleEntryPoint() {
    PIMAGE_DOS_HEADER mainmod_imgbase = (PIMAGE_DOS_HEADER) GetModuleHandle(NULL);
    PIMAGE_NT_HEADERS mainmod_nt = (PIMAGE_NT_HEADERS) ((uint8_t*)mainmod_imgbase + mainmod_imgbase->e_lfanew);
    
    return (void*) ((uint8_t*) mainmod_imgbase + mainmod_nt->OptionalHeader.AddressOfEntryPoint);

}

/*
 * Enable DEBUG privilege for current process
 * Return 0 in case of error, 1 otherwise
 */
int enable_debug_privilege() {
    HANDLE hToken;
    LUID SeDebugNameValue;
    TOKEN_PRIVILEGES TokenPrivileges;
    int result = 0;

    if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        if(LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &SeDebugNameValue)) {
            TokenPrivileges.PrivilegeCount = 1;
            TokenPrivileges.Privileges[0].Luid = SeDebugNameValue;
            TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

            if(AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
                result = 1;
            }
        }
        CloseHandle(hToken);
    }

    return result;
}

/*
 * Return thread RIP register from main module
 * Must be called when the main thread is in "suspended" state
 */
#ifndef MAKEULONGLONG
#define MAKEULONGLONG(ldw, hdw) (((ULONGLONG)hdw << 32) | ((ldw) & 0xFFFFFFFF))
#endif

#ifndef MAXULONGLONG
#define MAXULONGLONG ((ULONGLONG)~((ULONGLONG)0))
#endif

void* getMainThreadRip() {
    CONTEXT x64cpu = {0};
    DWORD dwMainThreadID = 0, dwProcID = GetCurrentProcessId();
    ULONGLONG ullMinCreateTime = MAXULONGLONG;
    THREADENTRY32 th32;
    FILETIME afTimes[4] = {0};
    HANDLE hThread;
    void* result = NULL;

    // Loop through current process threads
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

    if (hThreadSnap == INVALID_HANDLE_VALUE)
        return NULL;

    th32.dwSize = sizeof(THREADENTRY32);
    BOOL bOK = TRUE;
    for (bOK = Thread32First(hThreadSnap, &th32); bOK; bOK = Thread32Next(hThreadSnap, &th32)) {
        if (th32.th32OwnerProcessID == dwProcID) {
            hThread = OpenThread(THREAD_QUERY_INFORMATION, TRUE, th32.th32ThreadID);
            if (hThread) {
                if (GetThreadTimes(hThread, &afTimes[0], &afTimes[1], &afTimes[2], &afTimes[3])) {
                    ULONGLONG ullTest = MAKEULONGLONG(afTimes[0].dwLowDateTime, afTimes[0].dwHighDateTime);
                    if (ullTest && ullTest < ullMinCreateTime) {
                        ullMinCreateTime = ullTest;
                        dwMainThreadID = th32.th32ThreadID; // Main thread should be the oldest created one
                    }
                }
                CloseHandle(hThread);
            }
        }
    }

    CloseHandle(hThreadSnap);

    // Enable debug priviliges for current process, open target main thread
    // and grab CPU context (it is reliable/frozen as the thread should be in suspended state)
    if (dwMainThreadID && enable_debug_privilege()) {
        g_hMainThread = OpenThread(THREAD_QUERY_INFORMATION | THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME, FALSE, dwMainThreadID);

        if (g_hMainThread) {
            x64cpu.ContextFlags = CONTEXT_ALL;
            if (GetThreadContext(g_hMainThread, &x64cpu)) {
                result = (void*) x64cpu.Rip;
            }
        }
    }

    return result;
}

/*
 * Hooking based on int1 instruction + exception handler
 * Return 0 in case of failure
 */
int qbdipreload_hook(void* va) {
    if(!setInt1Exception(va)) {
        return 0;
    }

    return !setExceptionHandler(QbdiPreloadExceptionFilter);
}

/*
 * Attach mode initialization
 * Shared memory is initialized to make the QBDIPreload
 * able to access data from host injector
 */
BOOLEAN qbdipreload_attach_init() {
    TCHAR szShMemName[32];
    BOOL result = FALSE;

    snprintf(szShMemName, sizeof(szShMemName), QBDIPRELOAD_SHARED_MEMORY_NAME_FMT, GetCurrentProcessId());
    g_hShMemMap = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, szShMemName);
    g_pShMem = NULL;

    if (g_hShMemMap) {
        g_bIsAttachMode = TRUE;

        g_pShMem = MapViewOfFile(g_hShMemMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
        if (g_pShMem) {
            result = TRUE;
        }
    } else {
        g_bIsAttachMode = FALSE;
    }
    return result;
}

/*
 * Read data from shared memory
 */
BOOLEAN qbdipreload_read_shmem(LPVOID lpData, DWORD dwMaxBytesRead) {

    if(lpData && dwMaxBytesRead && g_pShMem && (dwMaxBytesRead <= SH_MEM_SIZE)) {
        memcpy(lpData, g_pShMem, dwMaxBytesRead);
        return TRUE;
    }

    return FALSE;
}

/*
 * Attach mode closing
 */
void qbdipreload_attach_close() {
    if (g_pShMem) {
        UnmapViewOfFile(g_pShMem);
        g_pShMem = NULL;
    }

    if (g_hShMemMap) {
        CloseHandle(g_hShMemMap);
        g_hShMemMap = NULL;
    }
}

/*
 * QBDI windows preload installation is done automatically
 * through DLLMain() once the QBDI instrumentation module
 * is loaded inside target (e.g. with LoadLibrary)
 * This function is automatically called when user use
 * QBDIPRELOAD_INIT macro
 */
BOOLEAN qbdipreload_hook_init(DWORD nReason) {
    if(nReason == DLL_PROCESS_ATTACH) {
        void* hook_target_va;

        if (g_bIsAttachMode) {
            hook_target_va = getMainThreadRip();
        }
        else {
            hook_target_va = getMainModuleEntryPoint();
        }
        // Call user provided callback on start
        int status = qbdipreload_on_start(hook_target_va);
        if (status == QBDIPRELOAD_NOT_HANDLED) {
            // QBDI preload installation
            qbdipreload_hook(hook_target_va);
            if (g_bIsAttachMode && g_hMainThread) {
                ResumeThread(g_hMainThread);
            }
        }
    }
    // Call user provided exit callback on DLL unloading
    else if(nReason == DLL_PROCESS_DETACH) {
        DWORD dwExitCode;
        GetExitCodeProcess(GetCurrentProcess(), &dwExitCode);
        qbdipreload_on_exit(dwExitCode);
        if (g_bIsAttachMode) {
            qbdipreload_attach_close();
        }
    }

    return TRUE;
}
