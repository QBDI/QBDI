#include "stdio.h"
#include "string.h"
#include "windows.h"

int main(int argc, TCHAR **argv) {
  STARTUPINFOW si = {0};
  PROCESS_INFORMATION pi = {0};
  int argCount;
  si.cb = sizeof(si);

  LPWSTR *argList = CommandLineToArgvW(GetCommandLineW(), &argCount);
  if (!argList) {
    printf("Failed to retrieve commandline arguments\n");
    return -1;
  }

  printf("QBDI Windows Preloader Tool\n");
  printf("---------------------------\n\n");

  if (argCount < 3) {
    printf(
        "Usage: QBDIWinPreloader <library> <executable> [<parameters> "
        "...]\n");
    LocalFree(argList);
    return -1;
  }

  wchar_t library[MAX_PATH] = {0};
  wcscpy_s(library, MAX_PATH, argList[1]);

  wchar_t target[MAX_PATH] = {0};
  wcscpy_s(target, MAX_PATH, argList[2]);

  wchar_t targetCmdLine[MAX_PATH] = {0};
  for (int i = 3; i < argCount; i++) {
    wcscat_s(targetCmdLine, MAX_PATH, L" ");
    wcscat_s(targetCmdLine, MAX_PATH, argList[i]);
  }

  printf("Target: %ls\n", target);
  printf("Target CommandLine: %ls\n", targetCmdLine);
  printf("Library: %ls\n", library);

  if (GetFileAttributesW(library) == INVALID_FILE_ATTRIBUTES) {
    printf("Failed to find library(%ls)\n", library);
    return -1;
  }

  BOOL cRes = CreateProcessW(target, targetCmdLine, NULL, NULL, TRUE,
                             CREATE_SUSPENDED, NULL, NULL, &si, &pi);
  if (!cRes) {
    printf("Process start failed(%ld)\n", GetLastError());
    return -1;
  }

  LPVOID procLoadLibrary =
      (LPVOID)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW");
  LPVOID memLibStr = (LPVOID)VirtualAllocEx(
      pi.hProcess, NULL, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
  if (memLibStr == NULL) {
    printf("Failed to allocate memory in process(%ld)\n", GetLastError());
    TerminateProcess(pi.hProcess, -1);
    return -1;
  }

  if (WriteProcessMemory(pi.hProcess, memLibStr, (wchar_t *)library, MAX_PATH,
                         NULL) == 0) {
    printf("Failed to write process memory(%ld)\n", GetLastError());
    TerminateProcess(pi.hProcess, -1);
    return -1;
  }

  printf("Launching Library Main\n");
  HANDLE remoteThread = CreateRemoteThread(
      pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)procLoadLibrary, memLibStr,
      0, NULL);
  if (remoteThread == 0) {
    printf("Failed to create remote process thread(%ld)\n", GetLastError());
    TerminateProcess(pi.hProcess, -1);
    return -1;
  }

  if (WaitForSingleObject(remoteThread, INFINITE) == WAIT_FAILED) {
    printf("Wait for remote thread failed(%ld)\n", GetLastError());
    return 1;
  }

  printf("Library Main Finished\n");
  printf("Resuming Process\n");
  ResumeThread(pi.hThread);

  if (WaitForSingleObject(pi.hProcess, INFINITE) == WAIT_FAILED) {
    printf("Wait for main thread failed(%ld)\n", GetLastError());
    return 1;
  }

  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);
  LocalFree(argList);
  return 0;
}
