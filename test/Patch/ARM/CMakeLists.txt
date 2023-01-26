target_sources(
  QBDITest
  PRIVATE "${CMAKE_CURRENT_LIST_DIR}/ComparedExecutor_ARM.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/ComparedExecutor_Thumb.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/MemoryAccessTable_ARM.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/LLVMOperandInfo_ARM.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/Instr_Test_ARM.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/Instr_Test_Thumb.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/Patch_Test_ARM.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/Patch_Test_Thumb.cpp")
