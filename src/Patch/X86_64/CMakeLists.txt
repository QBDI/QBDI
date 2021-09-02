set(SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ExecBlockFlags_X86_64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/InstInfo_X86_64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/InstrRules_X86_64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Layer2_X86_64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/MemoryAccess_X86_64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PatchGenerator_X86_64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PatchRules_X86_64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Register_X86_64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RelocatableInst_X86_64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/TempManager_X86_64.cpp")

target_sources(QBDI_src INTERFACE "${SOURCES}")
