Architecture Support
====================

Our patching system uses a generic approach which means instruction support is not explicit but 
implicit. Only instructions that use the program counter register (``rip`` under x86-64 and ``pc`` 
under ARM) or modify the control flow needs patching and we hope the current patching rules cover 
all such cases. However some corner cases or bugs in the disassembly and assembly backend of LLVM 
might still cause troubles.

To guarantee that instrumented programs run smoothly and that no such bugs exist, we are running 
automated tests on a wide variety of binaries (see :ref:`developer-testing` for more details). From 
those tests we can establish an instruction coverage which provides an estimation of the instructions 
supported by QBDI. This estimation is far from complete because the other instructions were simply 
probably never encountered in the test 
suite.

Instructions below are listed using their LLVM MC naming. This naming convention distinguishes 
between size and operand variants of the same mnemonic: ``ADD64rm`` is a 64 bits add of a memory 
value to a register while ``ADD32ri8`` is a 32 bits add of a 8 bits immediate to a register.

Intel x86-64
------------

The x86-64 support is complete and mature although there are still some rare bugs linked to problems  
in the assembly and disassembly backend of LLVM. Only a small part of SIMD instructions are covered 
by our tests but we do not expect any problems with the uncovered ones because their semantic are
closely related with the covered ones.

Memory access information is only provided for general instructions, not for SIMD ones.

Instruction Coverage
^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

   ADC32ri8, ADC64ri8
   ADD16mi8, ADD16ri, ADD16rm, ADD32i32, ADD32mi8, ADD32mr, ADD32ri, ADD32ri8, ADD32rm, ADD32rr, ADD64i32, ADD64mi32, ADD64mi8, ADD64mr, ADD64ri32, ADD64ri8, ADD64rm, ADD64rr, ADD8rm, ADD8rr
   ADDSDrm, ADDSDrr
   AND16i16, AND16ri, AND16rm, AND32i32, AND32mi, AND32mi8, AND32mr, AND32ri, AND32ri8, AND32rm, AND32rr, AND64mi32, AND64ri8, AND64rm, AND64rr, AND8mi, AND8mr, AND8ri, AND8rm, AND8rr
   ANDPDrm
   BSR32rr, BSR64rr
   BSWAP32r, BSWAP64r
   BT32rr, BT64rr
   BTR32mr, BTR64mr
   BTS32mr, BTS64mr
   CALL64m, CALL64pcrel32, CALL64r
   CBW
   CDQ, CDQE
   CMOVA32rr, CMOVA64rr
   CMOVAE32rm, CMOVAE32rr, CMOVAE64rm, CMOVAE64rr
   CMOVB32rr, CMOVB64rr
   CMOVBE32rr, CMOVBE64rm, CMOVBE64rr
   CMOVE32rm, CMOVE32rr, CMOVE64rm, CMOVE64rr 
   CMOVG32rr, CMOVG64rr
   CMOVGE32rm, CMOVGE32rr, CMOVGE64rm
   CMOVL32rr, CMOVL64rr
   CMOVLE32rm, CMOVLE32rr, CMOVLE64rm, CMOVLE64rr
   CMOVNE32rm, CMOVNE32rr, CMOVNE64rm, CMOVNE64rr
   CMOVNS32rm, CMOVNS32rr, CMOVNS64rm, CMOVNS64rr
   CMOVS32rr, CMOVS64rr
   CMP16i16, CMP16mi, CMP16mi8, CMP16mr, CMP16ri, CMP16ri8, CMP16rm, CMP16rr, CMP32i32, CMP32mi, CMP32mi8, CMP32mr, CMP32ri, CMP32ri8, CMP32rm, CMP32rr, CMP64i32, CMP64mi32, CMP64mi8, CMP64mr, CMP64ri32, CMP64ri8, CMP64rm, CMP64rr, CMP8i8, CMP8mi, CMP8mr, CMP8ri, CMP8rm, CMP8rr
   CMPSB
   CMPXCHG32rm, CMPXCHG64rm
   CPUID
   CQO
   CVTSD2SSrm, CVTSD2SSrr
   CVTSI2SD64rm, CVTSI2SD64rr, CVTSI2SDrm, CVTSI2SDrr
   CVTSI2SS64rr, CVTSI2SSrr
   CVTSS2SDrr
   CVTTSD2SI64rr, CVTTSD2SIrm, CVTTSD2SIrr
   CVTTSS2SI64rr
   CWDE
   DEC32m
   DIV32m, DIV32r, DIV64m, DIV64r
   DIVR_FPrST0
   DIVSDrm, DIVSDrr
   DIVSSrm, DIVSSrr
   DIV_FPrST0, DIV_FST0r
   IDIV32m, IDIV32r, IDIV64m, IDIV64r
   ILD_F32m, ILD_F64m
   IMUL32r, IMUL32rm, IMUL32rmi, IMUL32rr, IMUL32rri, IMUL32rri8, IMUL64r, IMUL64rm, IMUL64rmi32, IMUL64rr, IMUL64rri32, IMUL64rri8
   JAE_1, JAE_4
   JA_1, JA_4
   JBE_1, JBE_4
   JB_1, JB_4
   JE_1, JE_4
   JGE_1, JGE_4
   JG_1, JG_4
   JLE_1, JLE_4
   JL_1, JL_4
   JMP64m, JMP64r
   JMP_1, JMP_4
   JNE_1, JNE_4
   JNS_1, JNS_4
   JO_1, JO_4
   JP_1
   JS_1, JS_4
   LD_F32m, LD_F80m, LD_Frr
   LEA64_32r, LEA64r
   LEAVE64
   LFENCE
   MAXSSrr
   MFENCE
   MOV16mi, MOV16mr, MOV32mi, MOV32mr, MOV32ri, MOV32rm, MOV32rr, MOV64mi32, MOV64mr, MOV64ri, MOV64ri32, MOV64rm, MOV64rr, MOV8mi, MOV8mr, MOV8ri, MOV8rr
   MOVAPDrr
   MOVAPSmr, MOVAPSrr
   MOVDQArm
   MOVDQUrm
   MOVSB
   MOVSDmr, MOVSDrm
   MOVSL
   MOVSQ
   MOVSSmr, MOVSSrm
   MOVSX16rm8, MOVSX32rm16, MOVSX32rm8, MOVSX32rr8, MOVSX64rm16, MOVSX64rm32, MOVSX64rm8, MOVSX64rr32, MOVSX64rr8
   MOVUPSmr
   MOVZX32rm16, MOVZX32rm8, MOVZX32rr16, MOVZX32rr8
   MUL32r, MUL64r
   MULSDrm, MULSDrr
   MULSSrm, MULSSrr
   MUL_F32m, MUL_FPrST0, MUL_FST0r
   NEG32r, NEG64r
   NOOP, NOOPL, NOOPW
   NOT32r, NOT64r, NOT8m
   OR16rr, OR32i32, OR32mi, OR32mi8, OR32mr, OR32ri, OR32ri8, OR32rm, OR32rr, OR64i32, OR64mi32, OR64mi8, OR64mr, OR64ri8, OR64rm, OR64rr, OR8i8, OR8mi, OR8mr, OR8ri, OR8rm, OR8rr
   POP64r
   PUSH64i32, PUSH64i8, PUSH64r, PUSH64rmm
   PXORrr
   RDTSC
   RETQ
   ROL32r1, ROL32rCL, ROL32ri, ROL64r1, ROL64ri, ROR16ri, ROR32ri, ROR64r1, ROR64ri
   ROUNDSDr
   SAR32r1, SAR32rCL, SAR32ri, SAR64r1, SAR64rCL, SAR64ri, SAR8ri
   SBB32ri8, SBB32rr, SBB64ri8, SBB64rr
   SCASB
   SETAEr
   SETAm, SETAr
   SETBEr
   SETBm, SETBr
   SETEm, SETEr
   SETGEr
   SETGr
   SETLEr
   SETLr
   SETNEm, SETNEr
   SETNPr
   SHL32rCL, SHL32ri, SHL64m1, SHL64mi, SHL64rCL, SHL64ri
   SHR16r1, SHR16ri, SHR32r1, SHR32rCL, SHR32ri, SHR64r1, SHR64rCL, SHR64ri, SHR8r1, SHR8ri
   SQRTSDr
   STOSB, STOSL, STOSQ
   ST_FP64m, ST_FP80m, ST_FPrr
   SUB32i32, SUB32mi8, SUB32mr, SUB32ri, SUB32ri8, SUB32rm, SUB32rr, SUB64i32, SUB64mi8, SUB64mr, SUB64ri32, SUB64ri8, SUB64rm, SUB64rr, SUB8mi
   SUBSDrm, SUBSDrr
   SYSCALL
   TEST16ri, TEST16rr, TEST32i32, TEST32mi, TEST32ri, TEST32rm, TEST32rr, TEST64mi32, TEST64ri32, TEST64rm, TEST64rr, TEST8i8, TEST8mi, TEST8ri, TEST8rm, TEST8rr
   UCOMISDrr
   UCOMISSrm, UCOMISSrr
   UCOM_FIr
   XADD32rm
   XCHG32rm, XCHG64rr
   XCH_F
   XGETBV
   XOR16rm, XOR32ri8, XOR32rm, XOR32rr, XOR64mr, XOR64rm, XOR64rr, XOR8i8, XOR8mr, XOR8ri, XOR8rr

ARM
---

The ARM support is currently limited to the ARM instruction set of ARMv6, there is no support for 
Thumb or Thumb2 yet. While this subset is quite small, it is enough to run pure ARM mode binary 
and demonstrate that complete ARM support is possible.

No memory access information is provided at the moment.

Instruction Coverage
^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

   ADCri, ADCrr
   ADDri, ADDrr, ADDrsi
   ANDri, ANDrr
   BICri, BICrsi
   BL, BLX, BX_RET
   Bcc # Encompass all conditionnal branching instructions
   CLZ
   CMNri, CMNzrr
   CMPri, CMPrr, CMPrsi
   EORri, EORrr, EORrsi, EORrsr
   LDMIA, LDMIA_UPD
   LDRB_POST_IMM, LDRB_PRE_IMM, LDRBi12, LDRBrs
   LDRD
   LDRH
   LDR_POST_IMM, LDR_PRE_IMM, LDRi12, LDRrs
   MLA
   MOVi, MOVr, MOVsi, MOVsr
   MUL
   MVNi, MVNr
   ORRri, ORRrr, ORRrsi, ORRrsr
   REV
   RSBri, RSBrr
   SBCri, SBCrr
   STMDB_UPD
   STMIA, STMIB
   STRB_POST_IMM, STRB_PRE_IMM, STRBi12, STRBrs
   STRD, STRD_PRE
   STR_POST_IMM, STR_PRE_IMM, STRi12, STRrs
   SUBri, SUBrr, SUBrsi
   TSTri, TSTrr
   UMULL
   UXTB
   VLDMDIA_UPD
   VLDRD
   VSTMDDB_UPD
