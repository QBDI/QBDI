target_sources(
  QBDITest PRIVATE "${CMAKE_CURRENT_LIST_DIR}/InMemoryAssembler.cpp"
                   "${CMAKE_CURRENT_LIST_DIR}/ShellcodeTester.cpp")

if(QBDI_ARCH_X86_64)
  include("${CMAKE_CURRENT_LIST_DIR}/X86_64/CMakeLists.txt")
elseif(QBDI_ARCH_X86)
  include("${CMAKE_CURRENT_LIST_DIR}/X86/CMakeLists.txt")
elseif(QBDI_ARCH_ARM)
  include("${CMAKE_CURRENT_LIST_DIR}/ARM/CMakeLists.txt")
elseif(QBDI_ARCH_AARCH64)
  include("${CMAKE_CURRENT_LIST_DIR}/AARCH64/CMakeLists.txt")
else()
  message(FATAL_ERROR "No stub for architecture ${QBDI_ARCH}")
endif()
