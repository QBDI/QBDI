include("${CMAKE_CURRENT_LIST_DIR}/context_switch/CMakeLists.txt")

if(QBDI_ARCH_X86)
  include("${CMAKE_CURRENT_LIST_DIR}/X86/CMakeLists.txt")
elseif(QBDI_ARCH_X86_64)
  include("${CMAKE_CURRENT_LIST_DIR}/X86_64/CMakeLists.txt")
endif()

# Add QBDI target
set(SOURCES "${CMAKE_CURRENT_LIST_DIR}/ExecBlock.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/ExecBlockManager.cpp")

target_sources(QBDI_src INTERFACE "${SOURCES}")
