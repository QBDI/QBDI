if(__add_qbdi_llvm)
  return()
endif()
set(__add_qbdi_llvm ON)

include(FetchContent)

# configure FetchContent
set(QBDI_LLVM_VERSION 10.0.1)

FetchContent_Declare(
  llvm
  URL "https://github.com/llvm/llvm-project/releases/download/llvmorg-${QBDI_LLVM_VERSION}/llvm-${QBDI_LLVM_VERSION}.src.tar.xz"
  URL_HASH
    "SHA256=c5d8e30b57cbded7128d78e5e8dad811bff97a8d471896812f57fa99ee82cdf3"
  DOWNLOAD_DIR "${CMAKE_SOURCE_DIR}/third-party/llvm-download")

FetchContent_GetProperties(llvm)
if(NOT llvm_POPULATED)
  FetchContent_Populate(llvm)

  set(CMAKE_CXX_STANDARD
      17
      CACHE STRING "USE CPP 17")
  set(LLVM_BUILD_TOOLS
      OFF
      CACHE BOOL "Disable LLVM_BUILD_TOOLS")
  set(LLVM_BUILD_UTILS
      OFF
      CACHE BOOL "Disable LLVM_BUILD_UTILS")
  set(LLVM_BUILD_TESTS
      OFF
      CACHE BOOL "Disable LLVM_BUILD_TESTS")
  set(LLVM_BUILD_BENCHMARKS
      OFF
      CACHE BOOL "Disable LLVM_BUILD_BENCHMARKS")
  set(LLVM_BUILD_EXAMPLES
      OFF
      CACHE BOOL "Disable LLVM_BUILD_EXAMPLES")
  set(LLVM_INCLUDE_TOOLS
      OFF
      CACHE BOOL "Disable LLVM_INCLUDE_TOOLS")
  set(LLVM_INCLUDE_UTILS
      OFF
      CACHE BOOL "Disable LLVM_INCLUDE_UTILS")
  set(LLVM_INCLUDE_TESTS
      OFF
      CACHE BOOL "Disable LLVM_INCLUDE_TESTS")
  set(LLVM_INCLUDE_BENCHMARKS
      OFF
      CACHE BOOL "Disable LLVM_INCLUDE_BENCHMARKS")
  set(LLVM_INCLUDE_EXAMPLES
      OFF
      CACHE BOOL "Disable LLVM_INCLUDE_EXAMPLES")
  set(LLVM_ENABLE_TERMINFO
      OFF
      CACHE BOOL "Disable LLVM_ENABLE_TERMINFO")
  set(LLVM_ENABLE_BINDINGS
      OFF
      CACHE BOOL "Disable LLVM_ENABLE_BINDINGS")
  set(LLVM_ENABLE_RTTI
      OFF
      CACHE BOOL "Disable LLVM_ENABLE_RTTI")
  set(LLVM_APPEND_VC_REV
      OFF
      CACHE BOOL "Disable LLVM_APPEND_VC_REV")
  set(LLVM_ENABLE_Z3_SOLVER
      OFF
      CACHE BOOL "Disable LLVM_ENABLE_Z3_SOLVER")

  if(QBDI_ARCH_X86)
    set(QBDI_LLVM_ARCH X86)
    set(LLVM_TARGET_ARCH
        X86
        CACHE STRING "set LLVM_ARCH")
    set(LLVM_TARGETS_TO_BUILD
        X86
        CACHE STRING "set LLVM_TARGETS_TO_BUILD")
    set(LLVM_BUILD_32_BITS
        ON
        CACHE BOOL "set LLVM_BUILD_32_BITS")

    if(QBDI_PLATFORM_OSX)
      set(LLVM_ENABLE_LIBCXX
          ON
          CACHE BOOL "set LLVM_ENABLE_LIBCXX")
      set(LLVM_DEFAULT_TARGET_TRIPLE
          i386-apple-darwin17.7.0
          CACHE STRING "set LLVM_DEFAULT_TARGET_TRIPLE")
    elseif(QBDI_PLATFORM_LINUX OR QBDI_PLATFORM_ANDROID)
      set(LLVM_DEFAULT_TARGET_TRIPLE
          i386-pc-linux
          CACHE STRING "set LLVM_DEFAULT_TARGET_TRIPLE")
    endif()

  elseif(QBDI_ARCH_X86_64)
    set(QBDI_LLVM_ARCH X86)
    set(LLVM_TARGET_ARCH
        X86
        CACHE STRING "set LLVM_ARCH")
    set(LLVM_TARGETS_TO_BUILD
        X86
        CACHE STRING "set LLVM_TARGETS_TO_BUILD")

    if(QBDI_PLATFORM_LINUX)
      set(LLVM_DEFAULT_TARGET_TRIPLE
          x86_64-pc-linux-gnu
          CACHE STRING "set LLVM_DEFAULT_TARGET_TRIPLE")
    endif()
  else()
    message(FATAL_ERROR "Unsupported LLVM Architecture.")
  endif()

  # check if llvm-tblgen-X is available
  find_program(LLVM_TABLEN_BIN NAMES llvm-tblgen-10)
  message(STATUS "LLVM Table Gen found: ${LLVM_TABLEN_BIN}")
  if(${LLVM_TABLEN_BIN_FOUND})
    set(LLVM_TABLEGEN
        "${LLVM_TABLEN_BIN}"
        CACHE STRING "force tablegen")
  endif()

  if(QBDI_CCACHE AND CCACHE_FOUND)
    set(LLVM_CCACHE_BUILD
        ON
        CACHE BOOL "Enable CCACHE in llvm")
  else()
    set(LLVM_CCACHE_BUILD
        OFF
        CACHE BOOL "Enable CCACHE in llvm")
  endif()

  if(QBDI_ASAN AND HAVE_FLAG_SANITIZE_ADDRESS)
    set(LLVM_USE_SANITIZER
        Address
        CACHE STRING "Enable ASAN")
  endif()

  if(ANDROID)
    execute_process(
      COMMAND
        ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/llvm/lib-Transform-CMakeLists.patch.txt
        "${llvm_SOURCE_DIR}/lib/Transforms/CMakeLists.txt"
      COMMAND
        ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/llvm/test-CMakeLists.patch.txt
        "${llvm_SOURCE_DIR}/test/CMakeLists.txt")
  endif()

  option(QBDI_LLVM_NATIVE_BUILD "Hack llvm native build" ON)
  # tbl-gen compilation need a native compilation.
  # we need to hack cmake/modules/CrossCompile.cmake:llvm_create_cross_target
  if(QBDI_LLVM_NATIVE_BUILD AND (CMAKE_CROSSCOMPILING OR ANDROID))
    set(${CMAKE_PROJECT_NAME}_NATIVE_BUILD "${CMAKE_BINARY_DIR}/QBDI_NATIVE")
  endif()

  add_subdirectory(${llvm_SOURCE_DIR} ${llvm_BINARY_DIR} EXCLUDE_FROM_ALL)

  if(QBDI_LLVM_NATIVE_BUILD AND (CMAKE_CROSSCOMPILING OR ANDROID))
    set(QBDI_SOURCE_DIR "${CMAKE_SOURCE_DIR}")
    set(CMAKE_SOURCE_DIR
        "${llvm_SOURCE_DIR}"
        CACHE STRING "" FORCE)

    llvm_create_cross_target("${CMAKE_PROJECT_NAME}" NATIVE "" Release
                             -DLLVM_CCACHE_BUILD=${LLVM_CCACHE_BUILD})

    set(CMAKE_SOURCE_DIR
        "${QBDI_SOURCE_DIR}"
        CACHE STRING "" FORCE)
  endif()
endif()

# list of LLVM library to build
set(QBDI_LLVM_TARGET_LIBRARY)
set(QBDI_LLVM_LINK_LIBRARY)
macro(add_llvm_lib)
  foreach(LIB ${ARGV})
    if((TARGET ${LIB}) AND NOT (${LIB} IN_LIST QBDI_LLVM_TARGET_LIBRARY))
      list(APPEND QBDI_LLVM_TARGET_LIBRARY ${LIB})
      get_target_property(_LIB_LINK ${LIB} INTERFACE_LINK_LIBRARIES)
      if(_LIB_LINK)
        add_llvm_lib(${_LIB_LINK})
      endif()
    elseif(NOT (TARGET ${LIB}) AND NOT (${LIB} IN_LIST QBDI_LLVM_LINK_LIBRARY))
      list(APPEND QBDI_LLVM_LINK_LIBRARY ${LIB})
    endif()
  endforeach()
endmacro()

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
  add_llvm_lib(LLVMX86AsmParser LLVMX86Disassembler LLVMX86Desc LLVMX86Info
               LLVMX86Utils)
else()
  message(FATAL_ERROR "Unsupported LLVM Architecture.")
endif()

if(QBDI_PLATFORM_OSX)
  find_package(Python3 REQUIRED COMPONENTS Interpreter)

  set(LLVMSupportFixName "${llvm_BINARY_DIR}/libLLVMSupportFix.a")
  add_custom_command(
    OUTPUT "${LLVMSupportFixName}"
    COMMAND
      "${Python3_EXECUTABLE}"
      "${CMAKE_CURRENT_SOURCE_DIR}/cmake/llvm/rename_object.py" -i
      $<TARGET_FILE:LLVMSupport> -o "${LLVMSupportFixName}" -r Memory.cpp.o 1 -r
      Error.cpp.o 1
    COMMENT "Fix LLVMSupport library"
    DEPENDS LLVMSupport
    VERBATIM)
  list(REMOVE_ITEM QBDI_LLVM_TARGET_LIBRARY LLVMSupport)
  list(APPEND QBDI_LLVM_TARGET_LIBRARY "${LLVMSupportFixName}")

  set(LLVMObjectFixName "${llvm_BINARY_DIR}/libLLVMObjectFix.a")
  add_custom_command(
    OUTPUT "${LLVMObjectFixName}"
    COMMAND
      "${Python3_EXECUTABLE}"
      "${CMAKE_CURRENT_SOURCE_DIR}/cmake/llvm/rename_object.py" -i
      $<TARGET_FILE:LLVMObject> -o "${LLVMObjectFixName}" -r Minidump.cpp.o 1
    COMMENT "Fix LLVMObject library"
    DEPENDS LLVMObject
    VERBATIM)
  list(REMOVE_ITEM QBDI_LLVM_TARGET_LIBRARY LLVMObject)
  list(APPEND QBDI_LLVM_TARGET_LIBRARY "${LLVMObjectFixName}")

  list(APPEND QBDI_LLVM_LINK_LIBRARY -lc++)
else(QBDI_PLATFORM_LINUX)
  list(APPEND QBDI_LLVM_LINK_LIBRARY -lstdc++)
endif()

merge_static_libs(qbdi-llvm qbdi-llvm \${QBDI_LLVM_TARGET_LIBRARY})
target_link_libraries(qbdi-llvm INTERFACE ${QBDI_LLVM_LINK_LIBRARY})

target_include_directories(
  qbdi-llvm
  INTERFACE ${llvm_SOURCE_DIR}/include
  INTERFACE ${llvm_BINARY_DIR}/include
  INTERFACE ${llvm_SOURCE_DIR}/lib/Target/${QBDI_LLVM_ARCH}
  INTERFACE ${llvm_BINARY_DIR}/lib/Target/${QBDI_LLVM_ARCH}
  INTERFACE ${llvm_SOURCE_DIR}/lib
  INTERFACE ${llvm_BINARY_DIR}/lib)

add_custom_target(llvm DEPENDS qbdi-llvm)
