set(SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/AllocTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RangeTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/VMTest.cpp"
)

if (QBDI_ARCH_X86_64 OR QBDI_ARCH_X86)
    set(SOURCES ${SOURCES} "${CMAKE_CURRENT_LIST_DIR}/MemoryAccessTest.cpp")

    if (QBDI_ARCH_X86_64)
        set(SOURCES ${SOURCES} "${CMAKE_CURRENT_LIST_DIR}/InstAnalysisTest_X86_64.cpp"
                               "${CMAKE_CURRENT_LIST_DIR}/MemoryAccessTest_X86_64.cpp")
    endif()

    if (QBDI_ARCH_X86)
        set(SOURCES ${SOURCES} "${CMAKE_CURRENT_LIST_DIR}/InstAnalysisTest_X86.cpp"
                               "${CMAKE_CURRENT_LIST_DIR}/MemoryAccessTest_X86.cpp")
    endif()
endif()

target_sources(QBDITest PUBLIC "${SOURCES}")
