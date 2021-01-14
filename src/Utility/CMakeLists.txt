# Add QBDI target
set(SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Assembly.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/InstAnalysis.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/LogSys.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Memory.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Memory_${QBDI_PLATFORM}.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/String.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/System.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Version.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/memory_ostream.cpp"
    )

if (QBDI_ARCH_X86 OR QBDI_ARCH_X86_64)
    set(SOURCES
        ${SOURCES}
        "${CMAKE_CURRENT_LIST_DIR}/InstAnalysis_X86_64.cpp"
        )
endif()

if(QBDI_PLATFORM_IOS)
    set(SOURCES
        ${SOURCES}
        "${CMAKE_CURRENT_LIST_DIR}/System_iOS.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/server-iOS-jit-user.c"
        )
else()
    set(SOURCES
        ${SOURCES}
        "${CMAKE_CURRENT_LIST_DIR}/System_generic.cpp"
        )
endif()

target_sources(QBDI_src INTERFACE "${SOURCES}")

