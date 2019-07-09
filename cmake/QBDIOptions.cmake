
if(__add_qbdi_options)
	return()
endif()
set(__add_qbdi_options ON)

option(FORCE_DISABLE_AVX "Force disable AVX support in case dynamic support detection is buggy" OFF)
option(ASAN "Enable AddressSanitizer (ASAN) for debugging (May be slow down)" OFF)

option(LOG_DEBUG "Enable Debug log level" OFF)

# test (need gtest)
option(QBDITEST "Compile tests" ON)

# example
option(EXAMPLES "Compile examples" OFF)

# tools
option(TOOLS_QBDIPRELOAD "Compile QBDIPRELOAD (not available on windows)" ON)
option(TOOLS_VALIDATOR "Compile the validator (need TOOLS_QBDIPRELOAD)" OFF)
option(TOOLS_FRIDAQBDI "Install frida-qbdi" ON)

if((${PLATFORM} STREQUAL "linux-X86_64") OR (${PLATFORM} STREQUAL "macOS-X86_64"))
    option(TOOLS_PYQBDI "Compile python binding (need TOOLS_QBDIPRELOAD)" ON)
else()
    # PyQBDI may work on x86 but need python 32bits
    option(TOOLS_PYQBDI "Compile python binding (need TOOLS_QBDIPRELOAD)" OFF)
endif()

# package
option(PACKAGE_LIBNCURSE6 "create package with libncurse6 (default libncurse5)" OFF)
