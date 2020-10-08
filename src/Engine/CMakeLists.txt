# Add QBDI target
set(SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Engine.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/VM.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/VM_C.cpp"
    )

target_sources(QBDI_src INTERFACE "${SOURCES}")

