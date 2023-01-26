set(SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ExecBlockFlags_ARM.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ExecBlockPatch_ARM.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/InstInfo_ARM.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/InstrRules_ARM.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Layer2_ARM.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/MemoryAccess_ARM.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PatchCondition_ARM.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PatchGenerator_ARM.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PatchRuleAssembly_ARM.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Register_ARM.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RelocatableInst_ARM.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/TempManager_ARM.cpp")

target_sources(QBDI_src INTERFACE "${SOURCES}")
