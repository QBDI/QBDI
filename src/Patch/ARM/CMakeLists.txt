set(SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ExecBlockFlags_ARM.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/InstInfo_ARM.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/InstrRules_ARM.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Layer2_ARM.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PatchGenerator_ARM.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PatchRules_ARM.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RegisterSize_ARM.cpp")

target_sources(QBDI_src INTERFACE "${SOURCES}")
