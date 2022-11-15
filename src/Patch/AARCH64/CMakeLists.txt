set(SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ExecBlockFlags_AARCH64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ExecBlockPatch_AARCH64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/InstInfo_AARCH64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/InstrRules_AARCH64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Layer2_AARCH64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/MemoryAccess_AARCH64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PatchGenerator_AARCH64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PatchRuleAssembly_AARCH64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Register_AARCH64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RelocatableInst_AARCH64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/TempManager_AARCH64.cpp")

target_sources(QBDI_src INTERFACE "${SOURCES}")
