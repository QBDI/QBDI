target_sources(
  pyqbdi_module
  INTERFACE "${CMAKE_CURRENT_LIST_DIR}/Callback.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/Errors.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/InstAnalysis.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/Logs.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/Memory.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/Range.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/VM.cpp")

target_include_directories(pyqbdi_module INTERFACE ${CMAKE_CURRENT_LIST_DIR})

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
