# Add QBDI target
set(SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Context.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ExecBlock.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ExecBlockManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/${QBDI_PLATFORM}-${QBDI_ARCH}_Stub.${ASM_EXT}"
    )

# Assembly stubs
if (QBDI_PLATFORM_WINDOWS AND QBDI_BITS_32)
    set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/${QBDI_PLATFORM}-${QBDI_ARCH}_Stub.${ASM_EXT}" PROPERTIES COMPILE_FLAGS "/safeseh")
endif()

target_sources(QBDI_src INTERFACE "${SOURCES}")

