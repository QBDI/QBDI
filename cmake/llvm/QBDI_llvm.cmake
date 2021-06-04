if(__add_qbdi_llvm)
  return()
endif()
set(__add_qbdi_llvm ON)
include(ExternalProject)

# LLVM Parameter
# --------------
set(QBDI_LLVM_VERSION 10.0.1)
set(QBDI_LLVM_URL
  "https://github.com/llvm/llvm-project/releases/download/llvmorg-${QBDI_LLVM_VERSION}/llvm-${QBDI_LLVM_VERSION}.src.tar.xz")

# LLVM CACHE BUILD DIR
if(NOT DEFINED QBDI_LLVM_PREFIX)
  set(QBDI_LLVM_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/LLVM-${QBDI_LLVM_VERSION}")
  message(STATUS "QBDI_LLVM_PREFIX not specify. Use ${QBDI_LLVM_PREFIX}")
else()
  get_filename_component(QBDI_LLVM_PREFIX "${QBDI_LLVM_PREFIX}" REALPATH
                         BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
  file(MAKE_DIRECTORY "${QBDI_LLVM_PREFIX}")
  message(STATUS "QBDI_LLVM_PREFIX: ${QBDI_LLVM_PREFIX}")
endif()
set(QBDI_LLVM_BUILD_DIR "${QBDI_LLVM_PREFIX}/build")

# list of LLVM library to build
set(LLVM_NEEDED_LIBRARY)
set(LLVM_NEEDED_LIBRARY_TARGET)
set(LLVM_LINK_LIBRARY)
set(LLVM_MISSING_LIBRARY 0)
add_library(qbdi-llvm-build-deps INTERFACE)
macro(add_llvm_lib)
  foreach(LIB ${ARGV})
    if((TARGET ${LIB}) AND NOT (${LIB} IN_LIST LLVM_NEEDED_LIBRARY_TARGET))
      add_dependencies(${LIB} qbdi-llvm-build-deps)
      get_target_property(_LIB_NAME ${LIB} NAME)
      list(APPEND LLVM_NEEDED_LIBRARY ${_LIB_NAME})
      list(APPEND LLVM_NEEDED_LIBRARY_TARGET ${LIB})
      get_target_property(_LIB_PATH ${LIB} IMPORTED_LOCATION_RELEASE)
      if(NOT EXISTS "${_LIB_PATH}")
        set(LLVM_MISSING_LIBRARY 1)
      endif()
      get_target_property(_LIB_LINK ${LIB} INTERFACE_LINK_LIBRARIES)
      if(_LIB_LINK)
        add_llvm_lib(${_LIB_LINK})
      endif()
    elseif(NOT (TARGET ${LIB}) AND NOT (${LIB} IN_LIST LLVM_LINK_LIBRARY))
      list(APPEND LLVM_LINK_LIBRARY ${LIB})
    endif()
  endforeach()
endmacro()

# Exports can be found in <build>/cmake/modules/CMakeFiles/Export/lib/cmake/llvm/LLVMExports.cmake
if(QBDI_PLATFORM_LINUX)
  include(LLVMExportsLinux)
elseif(QBDI_PLATFORM_ANDROID)
  include(LLVMExportsAndroid)
elseif(QBDI_PLATFORM_WINDOWS)
  include(LLVMExportsWindows)
else()
  message(FATAL_ERROR "Missing LLVM Export plateform.")
endif()

add_llvm_lib(
  LLVMBinaryFormat
  LLVMMCDisassembler
  LLVMMCParser
  LLVMMC
  LLVMSupport
  LLVMObject
  LLVMTextAPI
  LLVMCore
  LLVMBitReader
  LLVMBitstreamReader
  LLVMRemarks)

if(QBDI_PLATFORM_OSX OR QBDI_PLATFORM_IOS)
  add_llvm_lib(LLVMDemangle)
endif()

if(QBDI_ARCH_X86_64 OR QBDI_ARCH_X86)
  add_llvm_lib(
    LLVMX86AsmParser
    LLVMX86Disassembler
    LLVMX86Desc
    LLVMX86Info
    LLVMX86Utils)
else()
  message(FATAL_ERROR "Unsupported LLVM Architecture.")
endif()

set(QBDI_LLVM_CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=Release
    -DLLVM_BUILD_TOOLS=off
    -DLLVM_BUILD_UTILS=off
    -DLLVM_BUILD_TESTS=off
    -DLLVM_BUILD_BENCHMARKS=off
    -DLLVM_BUILD_EXAMPLES=off
    -DLLVM_INCLUDE_TOOLS=off
    -DLLVM_INCLUDE_UTILS=off
    -DLLVM_INCLUDE_TESTS=off
    -DLLVM_INCLUDE_BENCHMARKS=off
    -DLLVM_INCLUDE_EXAMPLES=off
    -DLLVM_ENABLE_TERMINFO=off # have some link error on linux platform
    -DLLVM_ENABLE_BINDINGS=off
    -DLLVM_ENABLE_RTTI=off
    -DLLVM_APPEND_VC_REV=off
    -DLLVM_ENABLE_Z3_SOLVER=off
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER})

# set TARGET and TRIPLE
if(QBDI_ARCH_X86)
  list(APPEND QBDI_LLVM_CMAKE_ARGS -DLLVM_TARGET_ARCH=X86
       -DLLVM_TARGETS_TO_BUILD=X86 -DLLVM_BUILD_32_BITS=On)
  set(QBDI_LLVM_ARCH X86)

  if(QBDI_PLATFORM_OSX)
    list(APPEND QBDI_LLVM_CMAKE_ARGS
         -DLLVM_DEFAULT_TARGET_TRIPLE=i386-apple-darwin17.7.0)
  elseif(QBDI_PLATFORM_LINUX OR QBDI_PLATFORM_ANDROID)
    list(APPEND QBDI_LLVM_CMAKE_ARGS -DLLVM_DEFAULT_TARGET_TRIPLE=i386-pc-linux)
  endif()

elseif(QBDI_ARCH_X86_64)
  list(APPEND QBDI_LLVM_CMAKE_ARGS -DLLVM_TARGET_ARCH=X86
       -DLLVM_TARGETS_TO_BUILD=X86)
  set(QBDI_LLVM_ARCH X86)

  if(QBDI_PLATFORM_LINUX)
    list(APPEND QBDI_LLVM_CMAKE_ARGS
         -DLLVM_DEFAULT_TARGET_TRIPLE=x86_64-pc-linux-gnu)
  endif()
else()
  message(FATAL_ERROR "Unsupported LLVM Architecture.")
endif()

# check if llvm-tblgen-X is available
find_program(LLVM_TABLEN_BIN NAMES llvm-tblgen-10)
message(STATUS "LLVM Table Gen found: ${LLVM_TABLEN_BIN}")
if(${LLVM_TABLEN_BIN_FOUND})
  list(APPEND QBDI_LLVM_CMAKE_ARGS -DLLVM_TABLEGEN=${LLVM_TABLEN_BIN})
endif()

if(ANDROID)
  list(
    APPEND
    QBDI_LLVM_CMAKE_ARGS
    -DANDROID_TOOLCHAIN=${ANDROID_TOOLCHAIN}
    -DANDROID_NATIVE_API_LEVEL=${ANDROID_NATIVE_API_LEVEL}
    -DANDROID_ABI=${ANDROID_ABI}
    -DANDROID_PLATFORM=${ANDROID_PLATFORM}
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE})
endif()

if(QBDI_CCACHE AND CCACHE_FOUND)
  list(APPEND QBDI_LLVM_CMAKE_ARGS -DLLVM_CCACHE_BUILD=On)
endif()

if(QBDI_ASAN AND HAVE_FLAG_SANITIZE_ADDRESS)
  list(APPEND QBDI_LLVM_CMAKE_ARGS -DLLVM_USE_SANITIZER=Address)
endif()

ExternalProject_Add(
  qbdi-llvm-deps
  PREFIX ${QBDI_LLVM_PREFIX}
  URL ${QBDI_LLVM_URL}
  CMAKE_ARGS ${QBDI_LLVM_CMAKE_ARGS}
  BINARY_DIR ${QBDI_LLVM_BUILD_DIR}
  BUILD_COMMAND ${CMAKE_COMMAND} --build ${QBDI_LLVM_BUILD_DIR} --
                ${LLVM_NEEDED_LIBRARY}
  UPDATE_COMMAND ""
  INSTALL_COMMAND "")

if(ANDROID)
  ExternalProject_Add_Step(
    qbdi-llvm-deps patch-hello
    COMMENT "Patch lib/Transforms/CMakeLists.txt"
    COMMAND
      ${CMAKE_COMMAND} -E copy
      ${CMAKE_CURRENT_SOURCE_DIR}/cmake/llvm/lib-Transform-CMakeLists.patch.txt
      "<SOURCE_DIR>/lib/Transforms/CMakeLists.txt"
    COMMAND
      ${CMAKE_COMMAND} -E copy
      ${CMAKE_CURRENT_SOURCE_DIR}/cmake/llvm/test-CMakeLists.patch.txt
      "<SOURCE_DIR>/test/CMakeLists.txt"
    DEPENDEES configure)
endif()

ExternalProject_Get_Property(qbdi-llvm-deps SOURCE_DIR)
set(QBDI_LLVM_SOURCE_DIR "${SOURCE_DIR}")

add_custom_target(llvm DEPENDS qbdi-llvm-deps)

if(LLVM_MISSING_LIBRARY)
  message(
    WARNING
      "Some llvm library are not found. Compile the target \"llvm\" before the main target"
  )
else()
  add_dependencies(qbdi-llvm-build-deps qbdi-llvm-deps)

  merge_static_libs(qbdi-llvm qbdi-llvm \${LLVM_NEEDED_LIBRARY_TARGET})
  add_dependencies(qbdi-llvm qbdi-llvm-build-deps)
  target_link_libraries(qbdi-llvm INTERFACE ${LLVM_LINK_LIBRARY})

  target_include_directories(
    qbdi-llvm
    INTERFACE ${QBDI_LLVM_SOURCE_DIR}/include
              ${QBDI_LLVM_SOURCE_DIR}/lib/Target/${QBDI_LLVM_ARCH}
              ${QBDI_LLVM_SOURCE_DIR}/lib
    INTERFACE ${QBDI_LLVM_BUILD_DIR}/lib/Target/${QBDI_LLVM_ARCH}
              ${QBDI_LLVM_BUILD_DIR}/lib ${QBDI_LLVM_BUILD_DIR}/include/)

endif()
