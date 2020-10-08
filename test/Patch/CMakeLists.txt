set(SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ComparedExecutor_${QBDI_ARCH}.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Instr_${QBDI_BASE_ARCH}Test.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Patch_${QBDI_BASE_ARCH}Test.cpp"
)

if (QBDI_PLATFORM_WINDOWS)
    if(QBDI_ARCH_X86_64)
        set(SOURCES ${SOURCES}
            "${CMAKE_CURRENT_LIST_DIR}/WIN64_RunRealExec.asm"
        )
    elseif (QBDI_ARCH_X86)
        set(SOURCES ${SOURCES}
            "${CMAKE_CURRENT_LIST_DIR}/WIN86_RunRealExec.asm"
        )
        set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/WIN86_RunRealExec.asm" PROPERTIES COMPILE_FLAGS "/safeseh")
    endif()
endif()

target_sources(QBDITest PUBLIC "${SOURCES}")
