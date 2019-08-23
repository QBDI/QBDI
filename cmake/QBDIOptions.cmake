
if(__add_qbdi_options)
	return()
endif()
set(__add_qbdi_options ON)

# Enable compilation using ccache
option(QBDI_CCACHE "Enable CCACHE" ON)

# FORCE_DISABLE_AVX - default is OFF
if(QBDI_ARCH_X86_64 OR QBDI_ARCH_X86)
    option(QBDI_DISABLE_AVX "Force disable AVX support in case dynamic support detection is buggy" OFF)
else()
    # AVX only available on X86 and X86-64
    set(QBDI_DISABLE_AVX OFF)
endif()

# ASAN option
option(QBDI_ASAN "Enable AddressSanitizer (ASAN) for debugging (May be slow down)" OFF)

# Enable the logging level debug
option(QBDI_LOG_DEBUG "Enable Debug log level" OFF)

# test (need gtest)
option(QBDI_TEST "Compile tests" ON)

# example
option(QBDI_EXAMPLES "Compile examples" OFF)

# tools

if (NOT (QBDI_PLATFORM_WINDOWS OR QBDI_PLATFORM_IOS))
    # QBDIPreload (not available on windows)
    option(QBDI_TOOLS_QBDIPRELOAD "Compile QBDIPRELOAD (not available on windows)" ON)

    # Validator (compare execution between QBDIPreload and ptrace)
    option(QBDI_TOOLS_VALIDATOR "Compile the validator (need TOOLS_QBDIPRELOAD)" OFF)
else()
    set(QBDI_TOOLS_QBDIPRELOAD OFF)
    set(QBDI_TOOLS_VALIDATOR OFF)
endif()

# PYQBDI (need a python 32bit for 32bit architecture)
if(QBDI_BITS_64 AND (QBDI_PLATFORM_WINDOWS OR QBDI_PLATFORM_LINUX OR QBDI_PLATFORM_OSX))
    option(QBDI_TOOLS_PYQBDI "Compile python binding" ON)
else()
    option(QBDI_TOOLS_PYQBDI "Compile python binding" OFF)
endif()

# binding QBDI for frida
option(QBDI_TOOLS_FRIDAQBDI "Install frida-qbdi" ON)

# package
option(QBDI_PACKAGE_LIBNCURSE6 "create package with libncurse6 (default libncurse5)" OFF)


message(STATUS "== QBDI Options ==")
message(STATUS "QBDI_CCACHE:           ${QBDI_CCACHE}")
if (QBDI_ARCH_X86_64 OR QBDI_ARCH_X86)
    message(STATUS "QBDI_DISABLE_AVX:      ${QBDI_DISABLE_AVX}")
endif()
message(STATUS "QBDI_ASAN:             ${QBDI_ASAN}")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(QBDI_LOG_DEBUG ON)
    message(STATUS "QBDI_LOG_DEBUG:        ${QBDI_LOG_DEBUG} (by CMAKE_BUILD_TYPE=Debug)")
else()
    message(STATUS "QBDI_LOG_DEBUG:        ${QBDI_LOG_DEBUG}")
endif()
message(STATUS "QBDI_TEST:             ${QBDI_TEST}")
message(STATUS "QBDI_EXAMPLES:         ${QBDI_EXAMPLES}")
if (NOT (QBDI_PLATFORM_WINDOWS OR QBDI_PLATFORM_IOS))
    message(STATUS "QBDI_TOOLS_QBDIPRELOAD: ${QBDI_TOOLS_QBDIPRELOAD}")
    message(STATUS "QBDI_TOOLS_VALIDATOR:  ${QBDI_TOOLS_VALIDATOR}")
endif()
message(STATUS "QBDI_TOOLS_PYQBDI:     ${QBDI_TOOLS_PYQBDI}")
message(STATUS "QBDI_TOOLS_FRIDAQBDI:  ${QBDI_TOOLS_FRIDAQBDI}")

message(STATUS "")

if (QBDI_LOG_DEBUG)
    set(QBDI_ENABLE_LOG_DEBUG 1)
endif()

