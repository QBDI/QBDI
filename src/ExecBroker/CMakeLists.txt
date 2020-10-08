# Add QBDI target
set(SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ExecBroker.cpp"
    )

target_sources(QBDI_src INTERFACE "${SOURCES}")

