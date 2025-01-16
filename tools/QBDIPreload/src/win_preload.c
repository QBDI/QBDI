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
#include <Windows.h>
#include <tlhelp32.h>
#include <winnt.h>
#include "shlwapi.h"
#include "QBDIPreload.h"

#pragma comment(lib, "Shlwapi.lib")

/* Consts */
#define QBDIPRELOAD_SHARED_MEMORY_NAME_FMT "qbdi_preload_%u"
static const unsigned long INST_INT1 =
    0x01CD; /* Instruction opcode for "INT 1"        */
static const unsigned long INST_INT1_MASK = 0xFFFF;
static const size_t QBDI_RUNTIME_STACK_SIZE =
    0x800000; /* QBDI shadow stack size                */
static const long MEM_PAGE_SIZE = 4096; /* Default page size on Windows */
static const unsigned int SH_MEM_SIZE =
    4096; /* Shared memory size (attach mode only) */

/* Trampoline spec (to be called from assembly stub) */
void qbdipreload_trampoline();

/* Globals */
static struct {
  void *va;
  uint64_t orig_bytes;
} g_EntryPointInfo; /* Main module EntryPoint (PE from host process) */
static PVOID g_hExceptionHandler; /* VEH for QBDI preload internals (break on
                                     EntryPoint)                          */
static rword
    g_firstInstructionVA; /* First instruction that will be executed by QBDI */
static rword
    g_lastInstructionVA; /* Last instruction that will be executed by QBDI */
PVOID g_shadowStackTop =
    NULL; /* QBDI shadow stack top pointer(decreasing address) */
#if defined(QBDI_ARCH_X86)
PVOID g_shadowStackBase = NULL; /* QBDI shadow stack base pointer */
#endif
static GPRState g_EntryPointGPRState; /* QBDI CPU GPR states when EntryPoint has
                                         been reached */
static FPRState g_EntryPointFPRState; /* QBDI CPU FPR states when EntryPoint has
                                         been reached */
static HANDLE g_hMainThread; /* Main thread handle (attach mode only) */
static HANDLE g_hShMemMap;   /* Shared memory object between qbdipreload &
                                external binary (attach mode only) */
static LPVOID g_pShMem; /* Shared memory base pointer (attach mode only)      */
static BOOL g_bIsAttachMode; /* TRUE if attach mode is activated */

/*
 * Conversion from windows CONTEXT ARCH dependent structure
 * to QBDI GPR state (Global purpose registers)
 */
void qbdipreload_threadCtxToGPRState(const void *gprCtx, GPRState *gprState) {
  PCONTEXT osCpuCtx = (PCONTEXT)gprCtx;

#if defined(QBDI_ARCH_X86_64)
  gprState->rax = osCpuCtx->Rax;
  gprState->rbx = osCpuCtx->Rbx;
  gprState->rcx = osCpuCtx->Rcx;
  gprState->rdx = osCpuCtx->Rdx;
  gprState->rsi = osCpuCtx->Rsi;
  gprState->rdi = osCpuCtx->Rdi;
  gprState->rbp = osCpuCtx->Rbp;
  gprState->rsp = osCpuCtx->Rsp;
  gprState->r8 = osCpuCtx->R8;
  gprState->r9 = osCpuCtx->R9;
  gprState->r10 = osCpuCtx->R10;
  gprState->r11 = osCpuCtx->R11;
  gprState->r12 = osCpuCtx->R12;
  gprState->r13 = osCpuCtx->R13;
  gprState->r14 = osCpuCtx->R14;
  gprState->r15 = osCpuCtx->R15;
  gprState->rip = osCpuCtx->Rip;
#else
  gprState->eax = osCpuCtx->Eax;
  gprState->ebx = osCpuCtx->Ebx;
  gprState->ecx = osCpuCtx->Ecx;
  gprState->edx = osCpuCtx->Edx;
  gprState->esi = osCpuCtx->Esi;
  gprState->edi = osCpuCtx->Edi;
  gprState->ebp = osCpuCtx->Ebp;
  gprState->esp = osCpuCtx->Esp;
  gprState->eip = osCpuCtx->Eip;
#endif
  gprState->eflags = osCpuCtx->EFlags;
}

/*
 * Conversion from windows CONTEXT ARCH dependent structure
 * to QBDI FPR state (Floating point registers)
 */
void qbdipreload_floatCtxToFPRState(const void *gprCtx, FPRState *fprState) {
  PCONTEXT osCpuCtx = (PCONTEXT)gprCtx;

// FPU STmm(X)
#if defined(QBDI_ARCH_X86_64)
  memcpy_s(&fprState->stmm0, sizeof(MMSTReg),
           &osCpuCtx->FltSave.FloatRegisters[0], sizeof(MMSTReg));
  memcpy_s(&fprState->stmm1, sizeof(MMSTReg),
           &osCpuCtx->FltSave.FloatRegisters[1], sizeof(MMSTReg));
  memcpy_s(&fprState->stmm2, sizeof(MMSTReg),
           &osCpuCtx->FltSave.FloatRegisters[2], sizeof(MMSTReg));
  memcpy_s(&fprState->stmm3, sizeof(MMSTReg),
           &osCpuCtx->FltSave.FloatRegisters[3], sizeof(MMSTReg));
  memcpy_s(&fprState->stmm4, sizeof(MMSTReg),
           &osCpuCtx->FltSave.FloatRegisters[4], sizeof(MMSTReg));
  memcpy_s(&fprState->stmm5, sizeof(MMSTReg),
           &osCpuCtx->FltSave.FloatRegisters[5], sizeof(MMSTReg));
  memcpy_s(&fprState->stmm6, sizeof(MMSTReg),
           &osCpuCtx->FltSave.FloatRegisters[6], sizeof(MMSTReg));
  memcpy_s(&fprState->stmm7, sizeof(MMSTReg),
           &osCpuCtx->FltSave.FloatRegisters[7], sizeof(MMSTReg));
#else
  memcpy_s(&fprState->stmm0, sizeof(MMSTReg), &osCpuCtx->FloatSave.RegisterArea,
           sizeof(MMSTReg));
  memcpy_s(&fprState->stmm1, sizeof(MMSTReg),
           (LPBYTE)&osCpuCtx->FloatSave.RegisterArea + sizeof(MMSTReg),
           sizeof(MMSTReg));
  memcpy_s(&fprState->stmm2, sizeof(MMSTReg),
           (LPBYTE)&osCpuCtx->FloatSave.RegisterArea + 2 * sizeof(MMSTReg),
           sizeof(MMSTReg));
  memcpy_s(&fprState->stmm3, sizeof(MMSTReg),
           (LPBYTE)&osCpuCtx->FloatSave.RegisterArea + 3 * sizeof(MMSTReg),
           sizeof(MMSTReg));
  memcpy_s(&fprState->stmm4, sizeof(MMSTReg),
           (LPBYTE)&osCpuCtx->FloatSave.RegisterArea + 4 * sizeof(MMSTReg),
           sizeof(MMSTReg));
  memcpy_s(&fprState->stmm5, sizeof(MMSTReg),
           (LPBYTE)&osCpuCtx->FloatSave.RegisterArea + 5 * sizeof(MMSTReg),
           sizeof(MMSTReg));
  memcpy_s(&fprState->stmm6, sizeof(MMSTReg),
           (LPBYTE)&osCpuCtx->FloatSave.RegisterArea + 6 * sizeof(MMSTReg),
           sizeof(MMSTReg));
  memcpy_s(&fprState->stmm7, sizeof(MMSTReg),
           (LPBYTE)&osCpuCtx->FloatSave.RegisterArea + 7 * sizeof(MMSTReg),
           sizeof(MMSTReg));
#endif

#if defined(QBDI_ARCH_X86_64)
  // XMM(X) registers
  memcpy_s(&fprState->xmm0, 16, &osCpuCtx->Xmm0, 16);
  memcpy_s(&fprState->xmm1, 16, &osCpuCtx->Xmm1, 16);
  memcpy_s(&fprState->xmm2, 16, &osCpuCtx->Xmm2, 16);
  memcpy_s(&fprState->xmm3, 16, &osCpuCtx->Xmm3, 16);
  memcpy_s(&fprState->xmm4, 16, &osCpuCtx->Xmm4, 16);
  memcpy_s(&fprState->xmm5, 16, &osCpuCtx->Xmm5, 16);
  memcpy_s(&fprState->xmm6, 16, &osCpuCtx->Xmm6, 16);
  memcpy_s(&fprState->xmm7, 16, &osCpuCtx->Xmm7, 16);
  memcpy_s(&fprState->xmm8, 16, &osCpuCtx->Xmm8, 16);
  memcpy_s(&fprState->xmm9, 16, &osCpuCtx->Xmm9, 16);
  memcpy_s(&fprState->xmm10, 16, &osCpuCtx->Xmm10, 16);
  memcpy_s(&fprState->xmm11, 16, &osCpuCtx->Xmm11, 16);
  memcpy_s(&fprState->xmm12, 16, &osCpuCtx->Xmm12, 16);
  memcpy_s(&fprState->xmm13, 16, &osCpuCtx->Xmm13, 16);
  memcpy_s(&fprState->xmm14, 16, &osCpuCtx->Xmm14, 16);
  memcpy_s(&fprState->xmm15, 16, &osCpuCtx->Xmm15, 16);
#endif

// Others FPU registers
#if defined(QBDI_ARCH_X86_64)
  fprState->rfcw = osCpuCtx->FltSave.ControlWord;
  fprState->rfsw = osCpuCtx->FltSave.StatusWord;
  fprState->ftw = osCpuCtx->FltSave.TagWord;
  fprState->rsrv1 = osCpuCtx->FltSave.Reserved1;
  fprState->ip = (((uint64_t)osCpuCtx->FltSave.Reserved2) << 48) ||
                 (((uint64_t)osCpuCtx->FltSave.ErrorSelector) << 32) ||
                 ((uint64_t)osCpuCtx->FltSave.ErrorOffset);
  fprState->dp = (((uint64_t)osCpuCtx->FltSave.Reserved3) << 48) ||
                 (((uint64_t)osCpuCtx->FltSave.DataSelector) << 32) ||
                 ((uint64_t)osCpuCtx->FltSave.DataOffset);
  fprState->mxcsr = osCpuCtx->FltSave.MxCsr;
  fprState->mxcsrmask = osCpuCtx->FltSave.MxCsr_Mask;
#else
  fprState->rfcw = osCpuCtx->FloatSave.ControlWord;
  fprState->rfsw = osCpuCtx->FloatSave.StatusWord;
  fprState->ftw = osCpuCtx->FloatSave.TagWord;
  fprState->ip = osCpuCtx->FloatSave.ErrorOffset;
  fprState->cs = osCpuCtx->FloatSave.ErrorSelector;
  fprState->dp = osCpuCtx->FloatSave.DataOffset;
  fprState->ds = osCpuCtx->FloatSave.DataSelector;
  // Those are not available and set with default QBDI value
  fprState->mxcsr = 0x1f80;
  fprState->mxcsrmask = 0xffff;
#endif
}

/*
 * Write an "int 1" instruction at given address
 * Save previous byte to internal buffer
 * return 0 in case of failure
 */
int setInt1Exception(void *fn_va) {
  DWORD oldmemprot;

  if (!fn_va) {
    return 0;
  }

  uintptr_t base = (uintptr_t)fn_va - ((uintptr_t)fn_va % MEM_PAGE_SIZE);

  g_EntryPointInfo.va = fn_va;
  if (!VirtualProtect((void *)base, MEM_PAGE_SIZE, PAGE_READWRITE, &oldmemprot))
    return 0;
  g_EntryPointInfo.orig_bytes = *(uint64_t *)fn_va;
  *(uint64_t *)fn_va =
      INST_INT1 | (g_EntryPointInfo.orig_bytes & (~(uint64_t)INST_INT1_MASK));

  return VirtualProtect((void *)base, MEM_PAGE_SIZE, oldmemprot, &oldmemprot) !=
         0;
}

/*
 * Restore original bytes on a previously installed "int 1" instruction
 * return 0 in case of failure
 */
int unsetInt1Exception() {
  DWORD oldmemprot;
  uintptr_t base = (uintptr_t)g_EntryPointInfo.va -
                   ((uintptr_t)g_EntryPointInfo.va % MEM_PAGE_SIZE);

  if (!VirtualProtect((void *)base, MEM_PAGE_SIZE, PAGE_READWRITE, &oldmemprot))
    return 0;

  *(uint64_t *)g_EntryPointInfo.va = g_EntryPointInfo.orig_bytes;

  return VirtualProtect((void *)base, MEM_PAGE_SIZE, oldmemprot, &oldmemprot) !=
         0;
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
int setExceptionHandler(PVECTORED_EXCEPTION_HANDLER exception_filter_fn) {

  g_hExceptionHandler = AddVectoredExceptionHandler(1, exception_filter_fn);
  return g_hExceptionHandler != NULL;
}

/*
 * Remove some common window modules
 */
void removeConflictModule(VMInstanceRef vm, qbdi_MemoryMap *modules,
                          size_t size) {
  size_t i;
  for (i = 0; i < size; i++) {
    if ((modules[i].permission & QBDI_PF_EXEC) &&
        (StrStrIA(modules[i].name, "advapi") ||
         StrStrIA(modules[i].name, "combase") ||
         StrStrIA(modules[i].name, "comctl32") ||
         StrStrIA(modules[i].name, "comdlg") ||
         StrStrIA(modules[i].name, "gdi32") ||
         StrStrIA(modules[i].name, "gdiplus") ||
         StrStrIA(modules[i].name, "imm32") ||
         StrStrIA(modules[i].name, "kernel") ||
         StrStrIA(modules[i].name, "msvcp") ||
         StrStrIA(modules[i].name, "msvcrt") ||
         StrStrIA(modules[i].name, "ntdll") ||
         StrStrIA(modules[i].name, "ole32") ||
         StrStrIA(modules[i].name, "oleaut") ||
         StrStrIA(modules[i].name, "rpcrt") ||
         StrStrIA(modules[i].name, "sechost") ||
         StrStrIA(modules[i].name, "shcore") ||
         StrStrIA(modules[i].name, "shell32") ||
         StrStrIA(modules[i].name, "shlwapi") ||
         StrStrIA(modules[i].name, "ucrtbase") ||
         StrStrIA(modules[i].name, "user32") ||
         StrStrIA(modules[i].name, "uxtheme") ||
         StrStrIA(modules[i].name, "vcruntime") ||
         StrStrIA(modules[i].name, "win32u") || modules[i].name[0] == 0)) {
      qbdi_removeInstrumentedRange(vm, modules[i].start, modules[i].end);
    }
  }
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

  if (status == QBDIPRELOAD_NOT_HANDLED) {
    VMInstanceRef vm;
    qbdi_initVM(&vm, NULL, NULL, 0);
    qbdi_instrumentAllExecutableMaps(vm);

    size_t size = 0;
    qbdi_MemoryMap *modules = qbdi_getCurrentProcessMaps(false, &size);

    // Filter some modules to avoid conflicts
    qbdi_removeInstrumentedModuleFromAddr(vm,
                                          (rword)&qbdipreload_trampoline_impl);
    removeConflictModule(vm, modules, size);

    qbdi_freeMemoryMapArray(modules, size);

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
  PCONTEXT osCpuCtx = exc_info->ContextRecord;

  // Sanity check on exception
  if ((exc_info->ExceptionRecord->ExceptionCode ==
       EXCEPTION_ACCESS_VIOLATION) &&
#if defined(QBDI_ARCH_X86_64)
      (osCpuCtx->Rip == (DWORD64)g_EntryPointInfo.va)) {
#else
      (osCpuCtx->Eip == (DWORD)g_EntryPointInfo.va)) {
#endif
    // Call user provided callback with x64 cpu state (specific to windows)
    int status = qbdipreload_on_premain((void *)osCpuCtx, (void *)osCpuCtx);

    // Convert windows CPU context to QBDIGPR/FPR states
    qbdipreload_threadCtxToGPRState(osCpuCtx, &g_EntryPointGPRState);
    qbdipreload_floatCtxToFPRState(osCpuCtx, &g_EntryPointFPRState);

    // First instruction to execute is main module entry point)
    g_firstInstructionVA = QBDI_GPR_GET(&g_EntryPointGPRState, REG_PC);

    // In case if attach mode, it's difficult to guess on which instruction
    // stopping the instruction
    if (g_bIsAttachMode) {
      g_lastInstructionVA = (rword)0xFFFFFFFFFFFFFFFF;
    }
    // If start function has been hooked
    // Last instruction to execute is inside windows PE loader
    // (inside BaseThreadInitThunk() who called PE entry point & set RIP on
    // stack)
    else {
      g_lastInstructionVA =
          *((rword *)QBDI_GPR_GET(&g_EntryPointGPRState, REG_SP));
    }

    if (status == QBDIPRELOAD_NOT_HANDLED) {
      // Allocate shadow stack & keep some space at the end for QBDI runtime
      g_shadowStackTop = VirtualAlloc(NULL, QBDI_RUNTIME_STACK_SIZE,
                                      MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
      g_shadowStackTop = (PVOID)((uint64_t)g_shadowStackTop +
                                 QBDI_RUNTIME_STACK_SIZE - 0x1008);
#if defined(QBDI_ARCH_X86)
      g_shadowStackBase = g_shadowStackTop;
#endif
    }

// Continue execution on trampoline to make QBDI runtime
// execute using a separate stack and not instrumented target one
// RSP can't be set here (system seems to validate pointer authenticity)
#if defined(QBDI_ARCH_X86_64)
    osCpuCtx->Rip = (uint64_t)qbdipreload_trampoline;
#else
    osCpuCtx->Eip = (uint32_t)qbdipreload_trampoline;
#endif
  }

  return EXCEPTION_CONTINUE_EXECUTION;
}

/*
 * Get main module entry point
 */
void *getMainModuleEntryPoint() {
  PIMAGE_DOS_HEADER mainmod_imgbase = (PIMAGE_DOS_HEADER)GetModuleHandle(NULL);
  PIMAGE_NT_HEADERS mainmod_nt =
      (PIMAGE_NT_HEADERS)((uint8_t *)mainmod_imgbase +
                          mainmod_imgbase->e_lfanew);

  return (void *)((uint8_t *)mainmod_imgbase +
                  mainmod_nt->OptionalHeader.AddressOfEntryPoint);
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

  if (OpenProcessToken(GetCurrentProcess(),
                       TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
    if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &SeDebugNameValue)) {
      TokenPrivileges.PrivilegeCount = 1;
      TokenPrivileges.Privileges[0].Luid = SeDebugNameValue;
      TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

      if (AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges,
                                sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
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
#define MAKEULONGLONG(ldw, hdw) (((ULONGLONG)hdw << 32) | ((ldw)&0xFFFFFFFF))
#endif

#ifndef MAXULONGLONG
#define MAXULONGLONG ((ULONGLONG) ~((ULONGLONG)0))
#endif

void *getMainThreadRip() {
  CONTEXT osCpuCtx = {0};
  DWORD dwMainThreadID = 0, dwProcID = GetCurrentProcessId();
  ULONGLONG ullMinCreateTime = MAXULONGLONG;
  THREADENTRY32 th32;
  FILETIME afTimes[4] = {0};
  HANDLE hThread;
  void *result = NULL;

  // Loop through current process threads
  HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

  if (hThreadSnap == INVALID_HANDLE_VALUE)
    return NULL;

  th32.dwSize = sizeof(THREADENTRY32);
  BOOL bOK = TRUE;
  for (bOK = Thread32First(hThreadSnap, &th32); bOK;
       bOK = Thread32Next(hThreadSnap, &th32)) {
    if (th32.th32OwnerProcessID == dwProcID) {
      hThread = OpenThread(THREAD_QUERY_INFORMATION, TRUE, th32.th32ThreadID);
      if (hThread) {
        if (GetThreadTimes(hThread, &afTimes[0], &afTimes[1], &afTimes[2],
                           &afTimes[3])) {
          ULONGLONG ullTest = MAKEULONGLONG(afTimes[0].dwLowDateTime,
                                            afTimes[0].dwHighDateTime);
          if (ullTest && ullTest < ullMinCreateTime) {
            ullMinCreateTime = ullTest;
            dwMainThreadID = th32.th32ThreadID; // Main thread should be the
                                                // oldest created one
          }
        }
        CloseHandle(hThread);
      }
    }
  }

  CloseHandle(hThreadSnap);

  // Enable debug priviliges for current process, open target main thread
  // and grab CPU context (it is reliable/frozen as the thread should be in
  // suspended state)
  if (dwMainThreadID && enable_debug_privilege()) {
    g_hMainThread = OpenThread(THREAD_QUERY_INFORMATION | THREAD_GET_CONTEXT |
                                   THREAD_SUSPEND_RESUME,
                               FALSE, dwMainThreadID);

    if (g_hMainThread) {
      osCpuCtx.ContextFlags = CONTEXT_ALL;
      if (GetThreadContext(g_hMainThread, &osCpuCtx)) {
#if defined(QBDI_ARCH_X86_64)
        result = (void *)osCpuCtx.Rip;
#else
        result = (void *)osCpuCtx.Eip;
#endif
      }
    }
  }

  return result;
}

/*
 * Hooking based on int1 instruction + exception handler
 * Return 0 in case of failure
 */
int qbdipreload_hook(void *va) {
  if (!setInt1Exception(va)) {
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

  snprintf(szShMemName, sizeof(szShMemName), QBDIPRELOAD_SHARED_MEMORY_NAME_FMT,
           GetCurrentProcessId());
  g_hShMemMap =
      OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, szShMemName);
  g_pShMem = NULL;

  if (g_hShMemMap) {
    g_bIsAttachMode = TRUE;

    g_pShMem =
        MapViewOfFile(g_hShMemMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
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

  if (lpData && dwMaxBytesRead && g_pShMem && (dwMaxBytesRead <= SH_MEM_SIZE)) {
    memcpy_s(lpData, dwMaxBytesRead, g_pShMem, dwMaxBytesRead);
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
  if (nReason == DLL_PROCESS_ATTACH) {
    void *hook_target_va;

    if (g_bIsAttachMode) {
      hook_target_va = getMainThreadRip();
    } else {
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
  else if (nReason == DLL_PROCESS_DETACH) {
    DWORD dwExitCode;
    GetExitCodeProcess(GetCurrentProcess(), &dwExitCode);
    qbdipreload_on_exit(dwExitCode);
    if (g_bIsAttachMode) {
      qbdipreload_attach_close();
    }
  }

  return TRUE;
}
