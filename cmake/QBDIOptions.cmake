if(__add_qbdi_options)
  return()
endif()
set(__add_qbdi_options ON)

# Enable compilation using ccache
option(QBDI_CCACHE "Enable CCACHE or SCCACHE" ON)

# FORCE_DISABLE_AVX - default is OFF
if(QBDI_ARCH_X86_64 OR QBDI_ARCH_X86)
  option(QBDI_DISABLE_AVX
         "Force disable AVX support in case dynamic support detection is buggy"
         OFF)
else()
  # AVX only available on X86 and X86-64
  set(QBDI_DISABLE_AVX OFF)
endif()

# ASAN option
option(QBDI_ASAN
       "Enable AddressSanitizer (ASAN) for debugging (May be slow down)" OFF)

# Enable the logging level debug
option(QBDI_LOG_DEBUG "Enable Debug log level" OFF)

# Compile static Library
option(QBDI_STATIC_LIBRARY "Build the static library" ON)

# Compile shared Library
option(QBDI_SHARED_LIBRARY "Build the shared library" ON)

if(NOT QBDI_PLATFORM_ANDROID)
  # test
  option(QBDI_TEST "Compile tests" ON)

  # benchmark
  option(QBDI_BENCHMARK "Compile benchmark" OFF)
else()
  set(QBDI_TEST OFF)
  set(QBDI_BENCHMARK OFF)
endif()

# example
option(QBDI_EXAMPLES "Compile examples" OFF)

# tools

if(NOT (QBDI_PLATFORM_WINDOWS OR QBDI_PLATFORM_IOS))
  # QBDIPreload (not available on windows)
  option(QBDI_TOOLS_QBDIPRELOAD
         "Compile QBDIPRELOAD (not available on windows)" ON)

  # Validator (compare execution between QBDIPreload and ptrace)
  option(QBDI_TOOLS_VALIDATOR
         "Compile the validator (need QBDI_TOOLS_QBDIPRELOAD)" OFF)
else()
  set(QBDI_TOOLS_QBDIPRELOAD OFF)
  set(QBDI_TOOLS_VALIDATOR OFF)
endif()

# PYQBDI (need a python 32bit for 32bit architecture)
if(QBDI_BITS_64
   AND (QBDI_PLATFORM_WINDOWS
        OR QBDI_PLATFORM_LINUX
        OR QBDI_PLATFORM_OSX))
  option(QBDI_TOOLS_PYQBDI "Compile python binding" ON)
else()
  option(QBDI_TOOLS_PYQBDI "Compile python binding" OFF)
endif()

# binding QBDI for frida
option(QBDI_TOOLS_FRIDAQBDI "Install frida-qbdi" ON)

# verify options
if(NOT QBDI_STATIC_LIBRARY)
  if(QBDI_TEST)
    message(FATAL_ERROR "Need QBDI_STATIC_LIBRARY to compile QBDI_TEST")
  endif()
  if(QBDI_BENCHMARK)
    message(FATAL_ERROR "Need QBDI_STATIC_LIBRARY to compile QBDI_BENCHMARK")
  endif()
  if(QBDI_TOOLS_PYQBDI)
    message(FATAL_ERROR "Need QBDI_STATIC_LIBRARY to compile QBDI_TOOLS_PYQBDI")
  endif()
  if(QBDI_TOOLS_QBDIPRELOAD)
    message(
      FATAL_ERROR "Need QBDI_STATIC_LIBRARY to compile QBDI_TOOLS_QBDIPRELOAD")
  endif()
  if(QBDI_TOOLS_VALIDATOR)
    message(
      FATAL_ERROR "Need QBDI_STATIC_LIBRARY to compile QBDI_TOOLS_VALIDATOR")
  endif()
endif()

if(NOT QBDI_SHARED_LIBRARY)
  if(QBDI_EXAMPLES)
    message(FATAL_ERROR "Need QBDI_SHARED_LIBRARY to compile QBDI_EXAMPLES")
  endif()
endif()

if(QBDI_TOOLS_VALIDATOR AND NOT QBDI_TOOLS_QBDIPRELOAD)
  message(
    FATAL_ERROR "Need QBDI_TOOLS_QBDIPRELOAD to compile QBDI_TOOLS_VALIDATOR")
endif()

# display resulted options
message(STATUS "== QBDI Options ==")
message(STATUS "QBDI_CCACHE:           ${QBDI_CCACHE}")

if(QBDI_ARCH_X86_64 OR QBDI_ARCH_X86)
  message(STATUS "QBDI_DISABLE_AVX:      ${QBDI_DISABLE_AVX}")
endif()

message(STATUS "QBDI_ASAN:             ${QBDI_ASAN}")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(QBDI_LOG_DEBUG ON)
  message(
    STATUS
      "QBDI_LOG_DEBUG:        ${QBDI_LOG_DEBUG} (by CMAKE_BUILD_TYPE=Debug)")
else()
  message(STATUS "QBDI_LOG_DEBUG:        ${QBDI_LOG_DEBUG}")
endif()
message(STATUS "QBDI_STATIC_LIBRARY:   ${QBDI_STATIC_LIBRARY}")
message(STATUS "QBDI_SHARED_LIBRARY:   ${QBDI_SHARED_LIBRARY}")
message(STATUS "QBDI_TEST:             ${QBDI_TEST}")
message(STATUS "QBDI_BENCHMARK:        ${QBDI_BENCHMARK}")
message(STATUS "QBDI_EXAMPLES:         ${QBDI_EXAMPLES}")
if(NOT (QBDI_PLATFORM_WINDOWS OR QBDI_PLATFORM_IOS))
  message(STATUS "QBDI_TOOLS_QBDIPRELOAD: ${QBDI_TOOLS_QBDIPRELOAD}")
  message(STATUS "QBDI_TOOLS_VALIDATOR:  ${QBDI_TOOLS_VALIDATOR}")
endif()
message(STATUS "QBDI_TOOLS_PYQBDI:     ${QBDI_TOOLS_PYQBDI}")
message(STATUS "QBDI_TOOLS_FRIDAQBDI:  ${QBDI_TOOLS_FRIDAQBDI}")

message(STATUS "")

if(QBDI_LOG_DEBUG)
  set(QBDI_ENABLE_LOG_DEBUG 1)
endif()
