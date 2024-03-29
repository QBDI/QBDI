if(__add_qbdi_llvm_tblgen)
  return()
endif()
set(__add_qbdi_llvm_tblgen ON)

set(NATIVE_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/QBDI_llvm_tblgen_native")

add_custom_command(
  OUTPUT "${NATIVE_BUILD_DIR}" COMMAND ${CMAKE_COMMAND} -E make_directory
                                       "${NATIVE_BUILD_DIR}")

add_custom_target(CREATE_QBDI_TBLGEN_NATIVE_DIR DEPENDS "${NATIVE_BUILD_DIR}")

add_custom_command(
  OUTPUT "${NATIVE_BUILD_DIR}/CMakeCache.txt"
  COMMAND
    ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}"
    -DCMAKE_MAKE_PROGRAM="${CMAKE_MAKE_PROGRAM}"
    -DCMAKE_TOOLCHAIN_FILE="${QBDI_LLVM_TABLEN_TOOLSCHAIN}" ${llvm_SOURCE_DIR}
    -DLLVM_TARGET_IS_CROSSCOMPILE_HOST=TRUE
    -DLLVM_TARGETS_TO_BUILD="${LLVM_TARGETS_TO_BUILD}"
    -DLLVM_TARGET_ARCH="${LLVM_TARGET_ARCH}" -DCMAKE_BUILD_TYPE=Release
    -DLLVM_CCACHE_BUILD="${LLVM_CCACHE_BUILD}" -DLLVM_INCLUDE_TESTS=OFF
    -DLLVM_INCLUDE_BENCHMARKS=OFF -DLLVM_INCLUDE_EXAMPLES=OFF
  WORKING_DIRECTORY "${NATIVE_BUILD_DIR}"
  DEPENDS CREATE_QBDI_TBLGEN_NATIVE_DIR)

add_custom_target(CONFIGURE_QBDI_TBLGEN_NATIVE
                  DEPENDS "${NATIVE_BUILD_DIR}/CMakeCache.txt")

if(CMAKE_CONFIGURATION_TYPES)
  set(QBDI_LLVM_NATIVE_TBLGEN "${NATIVE_BUILD_DIR}/Release/bin/llvm-tblgen")
else()
  set(QBDI_LLVM_NATIVE_TBLGEN "${NATIVE_BUILD_DIR}/bin/llvm-tblgen")
endif()

add_custom_command(
  OUTPUT "${QBDI_LLVM_NATIVE_TBLGEN}"
  COMMAND ${CMAKE_COMMAND} --build "${NATIVE_BUILD_DIR}" --target llvm-tblgen
  DEPENDS CONFIGURE_QBDI_TBLGEN_NATIVE
  WORKING_DIRECTORY "${NATIVE_BUILD_DIR}"
  USES_TERMINAL)

add_custom_target(BUILD_QBDI_TBLGEN_NATIVE DEPENDS ${QBDI_LLVM_NATIVE_TBLGEN})
