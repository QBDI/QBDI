# Add QBDI target

if(QBDI_ARCH_X86 OR QBDI_ARCH_X86_64)
  include("${CMAKE_CURRENT_LIST_DIR}/X86_64/CMakeLists.txt")
elseif(QBDI_ARCH_ARM)
  include("${CMAKE_CURRENT_LIST_DIR}/ARM/CMakeLists.txt")
elseif(QBDI_ARCH_AARCH64)
  include("${CMAKE_CURRENT_LIST_DIR}/AARCH64/CMakeLists.txt")
endif()

set(SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/InstrRule.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/InstrRules.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/InstTransform.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PatchCondition.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PatchGenerator.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PatchUtils.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PatchRule.cpp")

target_sources(QBDI_src INTERFACE "${SOURCES}")
