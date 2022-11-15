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

Instructions below are listed using their mnemonic (or LLVM MC opcode). This naming convention distinguishes
between size and operand variants of the same mnemonic: ``ADD64rm`` is a 64 bits add of a memory
value to a register while ``ADD32ri8`` is a 32 bits add of an 8 bits immediate to a register.

AARCH64
-------

The AARCH64 support with the Memory Access.
The support is more recent that X86_64 and some bug should be expected.

For OSX, the register X18 is defined as platforms reserved. QBDI doesn't set or
used this register.

Local Monitor
^^^^^^^^^^^^^

The AARCH64 support includes an emulator of the Local Monitor. You can disable it
with the option :cpp:enumerator:`OPT_DISABLE_LOCAL_MONITOR`.

Instruction Coverage
^^^^^^^^^^^^^^^^^^^^

TODO

ARMv7
-----

The ARMv7 has been refactor to support both ARM and Thumb mode with the Memory Access.
The support is more recent that X86_64 and some bug should be expected.

Thumb Mode
^^^^^^^^^^

When the CPU is in Thumb Mode, the LSB of ``GPRState.PC`` is set to 1. However,
QBDI doesn't set the corrsponding byte in ``GPRState.CPSR``. In addition, when
an InstAnalysis is provided for a Thumb instruction, the ``address`` field
always has his LSB set to 0 and the field ``cpuMode`` must be used to distinguish
an ARM instruction from a Thumb instruction.

QBDI support the Thumb2 instruction IT. However, this instruction is handle
internally. The ``GPRState.CPSR`` doesn't include the ITSTATE. To avoid
undefined behavior, you must not:
- Clear the VM cache during the execution of an ITBlock.
- Start or restart the execution of QBDI in the middle of a ITBlock.

Local Monitor
^^^^^^^^^^^^^

The ARM support includes an emulator of the Local Monitor. You can disable it
with the option :cpp:enumerator:`OPT_DISABLE_LOCAL_MONITOR`.

Instruction Coverage
^^^^^^^^^^^^^^^^^^^^

.. code-block::

    ADCri
    ADDri ADDrr ADDrsi
    ANDri
    Bcc BL BX_RET
    CMNri
    CMPri CMPrr
    EORri
    FCONSTD
    FMSTAT
    LDMIA_UPD
    LDRBi12 LDRD LDRH LDRi12 LDR_PRE_IMM LDRrs
    MOVi MOVr MOVsi
    MVNi
    ORRri ORRrr
    STMDB_UPD
    STRBi12 STRBrs STRD STRi12
    SUBri
    UBFX
    UXTB UXTH
    VABSD VADD VCMPD VCMPE VCMPEZD VDIVD
    VLDMDIA_UPD VLDRD VLDRS
    VMLAD VMOVD VMOVDRR VMOVRRD
    VMOVRS VMOVSR VMULD VNMLSD VSITOD
    VSTMDDB_UPD VSTRD VSTRS
    VSUBD VTOSIZD VTOUIZD VUITOD

    tADC t2ADCri t2ADCrr t2ADCrs
    tADDhirr tADDi3 tADDi8 tADDrr tADDrSPi tADDspi t2ADDri t2ADDri12 t2ADDrr t2ADDrs t2ADDspImm t2ADDspImm12
    tADR t2ADR
    tAND t2ANDri t2ANDrr t2ANDrs
    tASRri tASRrr t2ASRri t2ASRrr
    tB tBcc tBL tBLXi tBLXr tBX t2B t2Bcc
    t2BICri t2BICrr t2BICrs
    t2CLZ
    tCBNZ tCBZ
    tCMNz t2CMNri t2CMNzrr
    tCMPhir tCMPi8 tCMPr t2CMPri t2CMPrr t2CMPrs
    t2DMB
    tEOR t2EORri t2EORrr t2EORrs
    tHINT t2HINT
    t2IT
    tLDMIA t2LDMIA t2LDMIA_UPD
    t2LDRBi12 t2LDRBi8
    tLDRBi tLDRBr t2LDRB_POST t2LDRB_PRE t2LDRBs
    t2LDRDi8
    t2LDREX
    tLDRHi tLDRHr t2LDRHi12 t2LDRHi8 t2LDRH_POST t2LDRHs
    tLDRi tLDRpci tLDRr tLDRspi t2LDRi12 t2LDRi8 t2LDRpci t2LDR_POST t2LDR_PRE t2LDRs
    tLDRSB t2LDRSBi12
    tLSLri tLSLrr tLSRri tLSRrr t2LSLri t2LSLrr t2LSRri t2LSRrr
    t2MLA t2MLS
    tMOVi8 tMOVr tMOVSr t2MOVi t2MOVi16 t2MOVTi16
    t2MRC
    t2MUL
    tMVN t2MVNi t2MVNr
    t2ORNri t2ORNrr
    tORR t2ORRri t2ORRrr t2ORRrs
    tPOP
    tPUSH
    tREV tREV16 t2REV
    t2RORri t2RSBri
    tRSB
    tSBC t2SBCri t2SBCrr t2SBCrs
    t2SMULL
    tSTMIA_UPD t2STMDB_UPD t2STMIA t2STMIA_UPD
    tSTRBi tSTRBr t2STRBi12 t2STRBi8 t2STRB_POST t2STRB_PRE t2STRBs
    t2STRDi8 t2STRD_PRE
    t2STREX
    tSTRHi tSTRHr t2STRHi12 t2STRHi8 t2STRH_PRE t2STRHs
    tSTRi TSTri tSTRr tSTRspi t2STRi12 t2STRi8 t2STR_POST t2STR_PRE t2STRs
    tSUBi3 tSUBi8 tSUBrr tSUBspi t2SUBri t2SUBri12 t2SUBrr t2SUBrs t2SUBspImm t2SUBspImm12
    tSXTB tSXTH
    t2TBB t2TBH
    t2TEQri
    tTST t2TSTri t2TSTrr
    t2UBFX
    t2UMLAL t2UMULL
    tUXTB
    tUXTH
    t2UXTAH t2UXTB

Intel x86-64
------------

The x86-64 support is complete and mature. Only a small part of SIMD instructions are covered
by our tests but we do not expect any problems with the uncovered ones because their semantic are
closely related to the covered ones. We currently don't support the following features:

- AVX512: the register of this extension isn't supported and will not be restored/backup during the execution
- privileged instruction: QBDI is an userland (ring3) application and privileged registers aren't managed
- CET feature: shadow stack is not implemented and the current instrumentation doesn't support
  indirect branch tracking.
- HLE and RTM features and any instructions for multithreading: QBDI allows inserting callbacks
  at any position and cannot guarantee that instructions of the same transactions unit will not be split.
- MPX feature: bound registers aren't supported.
- XOP instructions for ADM processors: The instructions are never been tested.

The memory access information is provided for most of general and SIMD instructions.
The information is missing for:

- The instructions include in an unsupported feature
- VGATHER* and VPGATHER* instructions of AVX2.

Instruction Coverage
^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

    ADC32mi8 ADC64mi8
    ADD16mi8 ADD16mr ADD16rm ADD32i32 ADD32mi8 ADD32mr ADD32ri ADD32ri8 ADD32rm ADD32rr ADD64i32 ADD64mi32 ADD64mi8 ADD64mr ADD64ri32 ADD64ri8 ADD64rm ADD64rr ADD8rr ADDSDrm_Int ADDSDrr_Int
    AESENCLASTrr AESENCrr
    AND16mi AND16mr AND32i32 AND32mi8 AND32mr AND32ri AND32ri8 AND32rm AND32rr AND64mi8 AND64mr AND64ri8 AND64rr AND8mi AND8mr AND8ri AND8rm AND8rr ANDNPDrr ANDPDrm ANDPDrr
    BSWAP32r BSWAP64r BT64rr
    CALL64m CALL64pcrel32 CALL64r
    CDQ CDQE
    CMOV32rm CMOV32rr CMOV64rm CMOV64rr
    CMP16mi8 CMP16mr CMP16rm CMP16rr CMP32i32 CMP32mi CMP32mi8 CMP32mr CMP32ri CMP32ri8 CMP32rm CMP32rr CMP64i32 CMP64mi32 CMP64mi8 CMP64mr CMP64ri32 CMP64ri8 CMP64rm CMP64rr CMP8i8 CMP8mi CMP8mr CMP8ri CMP8rm CMP8rr
    CMPSB
    CMPSDrr_Int
    CMPXCHG32rm CMPXCHG64rm
    COMISDrm COMISDrr
    CQO
    CVTSI2SDrr_Int CVTSI642SDrm_Int CVTSI642SDrr_Int CVTTSD2SI64rr_Int CVTTSD2SIrr_Int
    DEC32m DEC32r
    DIV32r DIV64m DIV64r DIVSDrm_Int DIVSDrr_Int
    IDIV32m IDIV32r IDIV64r
    IMUL32r IMUL32rm IMUL32rr IMUL64rmi8 IMUL64rr IMUL64rri32
    JCC_1 JCC_4
    JMP64m JMP64r
    JMP_1 JMP_4
    LEA64_32r LEA64r
    MFENCE
    MOV16mi MOV16mr MOV32mi MOV32mr MOV32ri MOV32rm MOV32rr MOV64mi32 MOV64mr MOV64ri MOV64ri32 MOV64rm MOV64rr MOV8mi MOV8mr
    MOVAPDrr MOVAPSmr
    MOVDQArm MOVDQArr
    MOVDQUmr MOVDQUrm
    MOVQI2PQIrm
    MOVSDmr MOVSDrm
    MOVSL MOVSQ
    MOVSX32rm8 MOVSX32rr8 MOVSX64rm32 MOVSX64rm8 MOVSX64rr16 MOVSX64rr32 MOVSX64rr8
    MOVUPSmr MOVUPSrm
    MOVZX32rm16 MOVZX32rm8 MOVZX32rr16 MOVZX32rr8
    MUL32r MUL64r MULSDrr_Int
    NEG32r NEG64r
    NOOP NOOP NOOPW
    NOT32r NOT64m NOT64r
    OR16rm OR32i32 OR32mi OR32mi8 OR32mr OR32ri OR32ri8 OR32rm OR32rr OR64i32 OR64ri8 OR64rm OR64rr OR8i8 OR8mi OR8mr OR8ri OR8rm OR8rr
    ORPDrr
    POP64r
    PSHUFBrr PSHUFDri
    PSLLDQri PSLLDri
    PUSH64i8 PUSH64r PUSH64rmm
    PXORrr
    RETQ
    ROL32r1 ROL32ri ROL64r1 ROL64ri
    ROR32ri ROR64r1 ROR64ri
    ROUNDSDr_Int
    SAR32r1 SAR32rCL SAR32ri SAR64r1 SAR64ri
    SBB32ri8 SBB32rr SBB64ri8 SBB64rr SBB8i8 SBB8ri
    SCASB
    SETCCm SETCCr
    SHL32rCL SHL32ri SHL64rCL SHL64ri
    SHR16ri SHR32r1 SHR32rCL SHR32ri SHR64r1 SHR64rCL SHR64ri SHR8r1 SHR8ri
    STOSQ
    SUB32mi8 SUB32mr SUB32ri SUB32ri8 SUB32rm SUB32rr SUB64mi8 SUB64mr SUB64ri32 SUB64ri8 SUB64rm SUB64rr SUB8mr SUBSDrm_Int SUBSDrr_Int
    SYSCALL
    TEST16mi TEST16ri TEST16rr TEST32i32 TEST32mi TEST32mr TEST32ri TEST32rr TEST64ri32 TEST64rr TEST8i8 TEST8mi TEST8ri TEST8rr
    UCOMISDrr
    VADDSDrm_Int VADDSDrr_Int
    VANDPDrr
    VCOMISDrr
    VFMADD132SDm_Int VFMADD132SDr_Int VFMADD213SDm_Int VFMADD231SDr_Int
    VFNMADD231SDm_Int
    VMOVAPDrr
    VMOVPDI2DIrr
    VMOVPQIto64rr
    VMOVQI2PQIrm
    VMOVSDrm
    VMULSDrm_Int VMULSDrr_Int
    VSTMXCSR
    VSUBSDrm_Int VSUBSDrr_Int
    VUCOMISDrm VUCOMISDrr
    VXORPDrr
    XADD32rm
    XCHG32rm XCHG64rr
    XOR32ri XOR32ri8 XOR32rm XOR32rr XOR64rm XOR64rr XOR8rm
    XORPSrr

Intel x86
---------

The x86 support is based on x86_64 and has the same limitations. However, its integration is more recent than X86_64
and has less been tested.

Instruction Coverage
^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

    ABS_F
    ADC32mi8 ADC32mr ADC32ri ADC32ri8 ADC32rm ADC32rr
    ADD16mi8 ADD16mr ADD16ri ADD16rm ADD32i32 ADD32mi ADD32mi8 ADD32mr ADD32ri ADD32ri8 ADD32rm ADD32rr ADD8rr
    ADD_F32m ADD_FPrST0 ADD_FrST0
    AESENCLASTrr AESENCrr
    AND16mi AND16mr AND32i32 AND32mi8 AND32mr AND32ri AND32ri8 AND32rm AND32rr AND8mi AND8mr AND8ri AND8rm AND8rr
    BSWAP32r
    BT32rr
    CALL32m CALL32r CALLpcrel32
    CDQ
    CHS_F
    CLD
    CMOV32rm CMOV32rr
    CMOVE_F
    CMP16mi8 CMP16mr CMP16rm CMP32i32 CMP32mi CMP32mi8 CMP32mr CMP32ri CMP32ri8 CMP32rm CMP32rr CMP8i8 CMP8mi CMP8mr CMP8ri CMP8rm CMP8rr
    CMPSB CMPSW
    CMPXCHG32rm
    COM_FIPr
    COM_FIr
    DEC32r_alt
    DIV32m DIV32r
    DIVR_F32m DIVR_F64m
    DIV_F32m DIV_F64m
    DIV_FPrST0
    FCOMP64m
    FLDCW16m
    FLDENVm
    FNSTCW16m FNSTSW16r
    FRNDINT
    FSTENVm
    FXAM
    IDIV32m
    ILD_F32m ILD_F64m
    IMUL32r IMUL32rm IMUL32rmi8 IMUL32rr IMUL32rri
    INC32r_alt
    IST_FP32m IST_FP64m
    JCC_1 JCC_4
    JMP32m JMP32r
    JMP_1 JMP_4
    LD_F0 LD_F1 LD_F32m LD_F64m LD_F80m LD_Frr
    LEA32r
    MOV16mi MOV16mr MOV16rm MOV32ao32 MOV32mi MOV32mr MOV32o32a MOV32ri MOV32rm MOV32rr MOV8mi MOV8mr MOV8rm MOV8rr
    MOVAPSrr
    MOVDQArm MOVDQArr
    MOVDQUmr
    MOVSB MOVSL
    MOVSX32rm8 MOVSX32rr16 MOVSX32rr8
    MOVUPSmr MOVUPSrm
    MOVZX32rm16 MOVZX32rm8 MOVZX32rr16 MOVZX32rr8
    MUL32m MUL32r
    MUL_F32m MUL_FPrST0 MUL_FST0r
    NEG32r
    NOOP
    NOT32m NOT32r
    OR16ri OR16rm OR32i32 OR32mi OR32mi8 OR32mr OR32ri OR32ri8 OR32rm OR32rr OR8i8 OR8mi OR8mr OR8ri OR8rm OR8rr
    POP32r
    PSHUFBrr PSHUFDri
    PSLLDQri PSLLDri
    PUSH32i8 PUSH32r PUSH32rmm PUSHi32
    PXORrr
    RETL
    ROL32r1 ROL32rCL ROL32ri
    ROR32ri
    SAHF
    SAR32r1 SAR32rCL SAR32ri
    SBB32mi8 SBB32ri8 SBB32rm SBB32rr SBB8i8 SBB8ri
    SCASB
    SETCCm SETCCr
    SHL32rCL SHL32ri
    SHLD32rrCL SHLD32rri8
    SHR16ri SHR32r1 SHR32rCL SHR32ri SHR8m1 SHR8ri
    SHRD32rri8
    STOSL
    ST_F64m ST_FP64m ST_FP80m ST_FPrr
    SUB32i32 SUB32mi8 SUB32mr SUB32ri SUB32ri8 SUB32rm SUB32rr SUB8mr SUB8ri SUB8rm
    SUBR_F64m SUBR_FPrST0
    SUB_FPrST0 SUB_FrST0
    TEST16mi TEST16ri TEST16rr TEST32i32 TEST32mi TEST32mr TEST32ri TEST32rr TEST8i8 TEST8mi TEST8mr TEST8ri TEST8rr
    UCOM_FIr UCOM_FPr
    XADD32rm
    XCHG32ar XCHG32rm XCHG32rr
    XCH_F
    XOR16rr XOR32i32 XOR32mr XOR32ri XOR32ri8 XOR32rm XOR32rr XOR8rm
    XORPSrr

