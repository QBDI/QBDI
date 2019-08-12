#include "QBDIPreload.h"
#include <Windows.h>
#include <winnt.h>

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
