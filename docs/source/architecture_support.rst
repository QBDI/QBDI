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

    ADC32mi8 ADC64mi8
    ADD16mi8 ADD16mr ADD16rm ADD32i32 ADD32mi8 ADD32mr ADD32ri ADD32ri8 ADD32rm ADD32rr ADD64i32 ADD64mi32 ADD64mi8 ADD64mr ADD64ri32 ADD64ri8 ADD64rm ADD64rr ADD8rr ADDSDrm
    AESENCLASTrr AESENCrr
    AND16mi AND16mr AND32i32 AND32mi8 AND32mr AND32ri AND32ri8 AND32rm AND32rr AND64mi8 AND64mr AND64ri8 AND64rr AND8mi AND8mr AND8ri AND8rm AND8rr ANDPDrr
    BSWAP32r BSWAP64r
    BT64rr
    CALL64m CALL64pcrel32 CALL64r
    CDQ
    CDQE
    CMOVA32rr CMOVA64rr CMOVAE32rm CMOVAE32rr CMOVAE64rr CMOVB32rr CMOVB64rr CMOVBE32rr CMOVBE64rm CMOVBE64rr CMOVE32rm CMOVE32rr CMOVE64rr CMOVG32rr CMOVG64rr CMOVGE32rr CMOVL32rr CMOVLE32rr CMOVNE32rm CMOVNE32rr CMOVNE64rm CMOVNE64rr CMOVNS32rr CMOVNS64rr CMOVS32rr CMOVS64rr
    CMP16mi8 CMP16mr CMP16rm CMP16rr CMP32i32 CMP32mi CMP32mi8 CMP32mr CMP32ri CMP32ri8 CMP32rm CMP32rr CMP64i32 CMP64mi32 CMP64mi8 CMP64mr CMP64ri32 CMP64ri8 CMP64rm CMP64rr CMP8i8 CMP8mi CMP8mr CMP8ri CMP8rm CMP8rr CMPSB
    CMPXCHG32rm CMPXCHG64rm
    COMISDrm COMISDrr
    CQO
    CVTSI2SDrr CVTSI642SDrr CVTTSD2SI64rr CVTTSD2SIrr
    DEC32r
    DIV64m DIV64r DIVSDrm DIVSDrr IDIV32m IDIV32r IDIV64r
    IMUL32r IMUL32rm IMUL32rr IMUL64rmi8 IMUL64rr IMUL64rri32
    JA_1 JA_4 JAE_1 JAE_4 JB_1 JB_4 JBE_1 JBE_4 JE_1 JE_4 JG_1 JG_4 JGE_1 JGE_4 JL_1 JL_4 JLE_1 JLE_4 JMP_1 JMP_4 JMP64m JMP64r JNE_1 JNE_4 JNP_1 JNS_1 JNS_4 JS_1 JS_4
    LEA64_32r LEA64r
    MFENCE
    MOV16mi MOV16mr MOV32mi MOV32mr MOV32ri MOV32rm MOV32rr MOV64mi32 MOV64mr MOV64ri MOV64ri32 MOV64rm MOV64rr MOV8mi MOV8mr MOVAPDrr MOVAPSmr MOVDQArm MOVDQArr MOVDQUmr MOVDQUrm MOVQI2PQIrm MOVSDmr MOVSDrm MOVSL MOVSQ MOVSX32rm8 MOVSX32rr8 MOVSX64rm32 MOVSX64rm8 MOVSX64rr16 MOVSX64rr32 MOVSX64rr8 MOVUPSmr MOVUPSrm MOVZX32rm16 MOVZX32rm8 MOVZX32rr16 MOVZX32rr8
    MUL32r MUL64r MULSDrr
    NEG32r NEG64r
    NOOP NOOPL NOOPW
    NOT32r NOT64m NOT64r
    OR16rm OR32i32 OR32mi OR32mi8 OR32mr OR32ri8 OR32rm OR32rr OR64i32 OR64ri8 OR64rm OR64rr OR8i8 OR8mi OR8mr OR8rm OR8rr
    POP64r
    PSHUFBrr PSHUFDri
    PSLLDQri PSLLDri
    PUSH64i32 PUSH64i8 PUSH64r PUSH64rmm
    PXORrr
    RETQ
    ROL32r1 ROL32ri ROL64r1 ROL64ri
    ROR32ri ROR64r1 ROR64ri
    SAR32r1 SAR32rCL SAR32ri SAR64r1 SAR64ri
    SBB32ri8 SBB32rr SBB64ri8 SBB64rr SBB8i8 SBB8ri
    SCASB
    SETAEr SETAr SETBEr SETBr SETEm SETEr SETGr SETLEr SETLr SETNEm SETNEr SETOr
    SHL32rCL SHL32ri SHL64rCL SHL64ri
    SHR16ri SHR32r1 SHR32rCL SHR32ri SHR64r1 SHR64rCL SHR64ri SHR8r1 SHR8ri
    STOSQ
    SUB32i32 SUB32mr SUB32ri SUB32ri8 SUB32rm SUB32rr SUB64mi8 SUB64mr SUB64ri32 SUB64ri8 SUB64rm SUB64rr
    SUB8mr
    SYSCALL
    TEST16mi TEST16ri TEST16rr TEST32i32 TEST32mi TEST32mr TEST32ri TEST32rr TEST64ri32 TEST64rr TEST8i8 TEST8mi TEST8ri TEST8rr
    UCOMISDrr
    VADDSDrm VADDSDrr
    VCVTSI2SDrr
    VFMADD132SDm VFMADD132SDr VFMADD213SDm VFMADD231SDm
    VFMSUB132SDr
    VMOVAPDrr VMOVPQIto64rr VMOVSDrm
    VMULSDrm VMULSDrr
    VSUBSDrr
    VXORPDrr
    XADD32rm
    XCHG32rm XCHG64rr
    XOR32ri XOR32ri8 XOR32rm XOR32rr XOR64rm XOR64rr XOR8rm XORPSrr

Intel x86
---------

The x86 support is based on x86-64.

Memory access information is only provided for general instructions, not for SIMD ones.

Instruction Coverage
^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

    ABS_F
    ADC32mi8 ADC32mr ADC32ri ADC32ri8 ADC32rm ADC32rr
    ADD16mi8 ADD16mr ADD16ri ADD16rm ADD32i32 ADD32mi ADD32mi8 ADD32mr ADD32ri ADD32ri8 ADD32rm ADD32rr ADD8rr ADD_F32m
    AESENCLASTrr AESENCrr
    AND16mi AND16mr AND32i32 AND32mi8 AND32mr AND32ri AND32ri8 AND32rm AND32rr AND8mi AND8mr AND8ri AND8rm AND8rr
    BSWAP32r
    BT32rr
    CALL32m CALL32r CALLpcrel32
    CDQ
    CHS_F
    CLD
    CMOVA32rr CMOVAE32rm CMOVAE32rr CMOVB32rm CMOVB32rr CMOVBE32rm CMOVBE32rr CMOVE32rm CMOVE32rr CMOVG32rm CMOVG32rr CMOVGE32rr CMOVL32rr CMOVLE32rr CMOVNE32rm CMOVNE32rr CMOVNS32rm CMOVNS32rr CMOVS32rr
    CMP16mi8 CMP16mr CMP16rm CMP16rr CMP32i32 CMP32mi CMP32mi8 CMP32mr CMP32ri CMP32ri8 CMP32rm CMP32rr CMP8i8 CMP8mi CMP8mr CMP8ri CMP8rm CMP8rr CMPSB CMPSW CMPXCHG32rm
    COM_FIPr COM_FIr
    DEC32r_alt
    DIV32m DIV32r DIV_F64m DIV_FPrST0 DIVR_F32m DIVR_F64m
    FCOMP64m
    FLDCW16m
    FNSTCW16m FNSTSW16r
    FXAM
    IDIV32m
    ILD_F32m
    ILD_F64m
    IMUL32r IMUL32rm IMUL32rmi8 IMUL32rr IMUL32rri
    INC32r_alt
    IST_FP32m IST_FP64m
    JA_1 JA_4 JAE_1 JAE_4 JB_1 JB_4 JBE_1 JBE_4 JE_1 JE_4 JG_1 JG_4 JGE_1 JGE_4 JL_1 JL_4 JLE_1 JLE_4 JMP_1 JMP32m JMP32r JMP_4 JNE_1 JNE_4 JNP_1 JNS_1 JNS_4 JS_1 JS_4
    LD_F0 LD_F1 LD_F32m LD_F64m LD_F80m LD_Frr
    LEA32r
    MOV16mi MOV16mr MOV16o32a MOV16rm MOV32ao32 MOV32mi MOV32mr MOV32o32a MOV32ri MOV32rm MOV32rr MOV8mi MOV8mr MOV8o32a MOV8rm MOV8rr MOVAPSrr MOVDQArm MOVDQArr MOVDQUmr MOVSB MOVSL MOVSX32rm8 MOVSX32rr16 MOVSX32rr8 MOVUPSmr MOVUPSrm MOVZX32rm16 MOVZX32rm8 MOVZX32rr16 MOVZX32rr8
    MUL32m MUL32r MUL_F32m MUL_FPrST0 MUL_FST0r
    NEG32r
    NOOP
    NOT32m NOT32r
    OR16rm OR32i32 OR32mi OR32mi8 OR32mr OR32ri8 OR32rm OR32rr OR8i8 OR8mi OR8mr OR8ri OR8rm OR8rr
    POP32r
    PSHUFBrr
    PSHUFDri
    PSLLDQri
    PSLLDri
    PUSH32i8 PUSH32r PUSH32rmm PUSHi32
    PXORrr
    RETL
    ROL32r1 ROL32rCL ROL32ri
    ROR32ri
    SAHF
    SAR32r1 SAR32rCL SAR32ri
    SBB32mi8 SBB32ri8 SBB32rm SBB32rr SBB8i8 SBB8ri
    SCASB
    SETAEr SETAr SETBEr SETBr SETEm SETEr SETGr SETLEr SETLr SETNEm SETNEr SETOr
    SHL32rCL SHL32ri SHLD32rrCL SHLD32rri8
    SHR16ri SHR32r1 SHR32rCL SHR32ri SHR8m1 SHR8r1 SHR8ri SHRD32rri8
    ST_F64m ST_FP64m ST_FP80m ST_FPrr
    STOSL
    SUB32i32 SUB32mi8 SUB32mr SUB32ri SUB32ri8 SUB32rm SUB32rr SUB8mr SUB8ri SUB8rm
    TEST16mi TEST16ri TEST16rr TEST32i32 TEST32mi TEST32mr TEST32ri TEST32rr TEST8i8 TEST8mi TEST8mr TEST8ri TEST8rr
    UCOM_FIr UCOM_FPr
    XADD32rm
    XCH_F
    XCHG32ar XCHG32rm XCHG32rr
    XOR16rr XOR32i32 XOR32mr XOR32ri XOR32ri8 XOR32rm XOR32rr XOR8rm XORPSrr

