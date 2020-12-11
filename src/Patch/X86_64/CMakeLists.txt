set(SOURCES
    "${CMAKE_CURRENT_LIST_DIR}//InstInfo_X86_64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}//RegisterSize_X86_64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}//PatchRules_X86_64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}//Layer2_X86_64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}//InstrRules_X86_64.cpp"
    "${CMAKE_CURRENT_LIST_DIR}//PatchGenerator_X86_64.cpp"
    )

target_sources(QBDI_src INTERFACE "${SOURCES}")

