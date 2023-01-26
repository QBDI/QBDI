target_sources(
  QBDITest
  PRIVATE "${CMAKE_CURRENT_LIST_DIR}/ComparedExecutor_X86_64.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/MemoryAccessTable_X86_64.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/LLVMOperandInfo_X86_64.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/Instr_Test_X86_64.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/Patch_Test_X86_64.cpp")

if(QBDI_PLATFORM_WINDOWS)
  target_sources(QBDITest
                 PRIVATE "${CMAKE_CURRENT_LIST_DIR}/WIN64_RunRealExec.asm")
endif()
