# Add QBDI target
set(SOURCES "${CMAKE_CURRENT_LIST_DIR}/Context_X86.cpp")

target_sources(QBDI_src INTERFACE "${SOURCES}")
