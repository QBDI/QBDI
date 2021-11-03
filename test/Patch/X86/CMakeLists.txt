target_sources(
  QBDITest
  PRIVATE "${CMAKE_CURRENT_LIST_DIR}/ComparedExecutor_X86.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/../X86_64/MemoryAccessTable_X86_64.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/Instr_Test_X86.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/Patch_Test_X86.cpp")

if(QBDI_PLATFORM_WINDOWS)
  target_sources(QBDITest
                 PRIVATE "${CMAKE_CURRENT_LIST_DIR}/WIN86_RunRealExec.asm")
  set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/WIN86_RunRealExec.asm"
                              PROPERTIES COMPILE_FLAGS "/safeseh")
endif()
