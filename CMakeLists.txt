cmake_minimum_required (VERSION 3.5)
project(QBDI)

# QBDI version
set(QBDI_VERSION_MAJOR 0)
set(QBDI_VERSION_MINOR 6)
set(QBDI_VERSION_PATCH 2)

set(SUPPORTED_PLATFORM linux-X86_64 linux-ARM macOS-X86_64 android-ARM
    win-X86_64 iOS-ARM linux-X86 macOS-X86 win-X86)

# Force find system utils (strip)
include(CMakeFindBinUtils)

# Macros
include(cmake/libutils.cmake)


# Process the PLATFORM variable

if(NOT DEFINED PLATFORM)
    message(FATAL_ERROR "
    Please define the platform to target using -DPLATFORM=.
    ")
endif()

list(FIND SUPPORTED_PLATFORM ${PLATFORM} IS_PLATFORM_SUPPORTED)

if(IS_PLATFORM_SUPPORTED EQUAL -1)
    message(FATAL_ERROR "
    ${PLATFORM} is not a supported architecture. Currently supported:
    ${SUPPORTED_PLATFORM}
    ")
endif()

# Extract variables (OS / ARCH) from PLATFORM variable
string(REGEX MATCH "(.*)-(.*)" _ ${PLATFORM})
set(OS ${CMAKE_MATCH_1})
set(ARCH ${CMAKE_MATCH_2})
if(${ARCH} STREQUAL "X86")
    set(BASE_ARCH "X86_64")
else()
    set(BASE_ARCH "${ARCH}")
endif()
string(REGEX MATCH "([A-Za-z0-9]*)_*" _ ${ARCH})
set(LLVM_ARCH ${CMAKE_MATCH_1})
message("OS: ${OS}")
message("ARCH: ${ARCH}")
message("LLVM_ARCH: ${LLVM_ARCH}")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
include(QBDIOptions)

if((LOG_DEBUG) OR (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    message(STATUS "Compiling with LOG_DEBUG")
    add_definitions(-D_QBDI_LOG_DEBUG)
endif()

if(FORCE_DISABLE_AVX)
    message(STATUS "Compiling with FORCE_DISABLE_AVX")
    add_definitions(-D_QBDI_FORCE_DISABLE_AVX)
endif()

include(CheckCCompilerFlag)

if (ASAN)
  set(CMAKE_REQUIRED_FLAGS "-Werror -fsanitize=address")
  check_c_compiler_flag("-fsanitize=address" HAVE_FLAG_SANITIZE_ADDRESS)
  unset(CMAKE_REQUIRED_FLAGS)
  if(HAVE_FLAG_SANITIZE_ADDRESS)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    message(STATUS "ASAN is activated")
  else()
    message(WARNING "ASAN can't be activated")
  endif()
endif()

# Configure RPATH on OS X

if(APPLE)
  set(CMAKE_MACOSX_RPATH TRUE)

  # use, i.e. don't skip the full RPATH for the build tree
  set(CMAKE_SKIP_BUILD_RPATH  FALSE)

  # when building, don't use the install RPATH already
  # (but later on when installing)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

  # add the automatically determined parts of the RPATH
  # which point to directories outside the build tree to the install RPATH
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

  # the RPATH to be used when installing, but only if it's not a system directory
  list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
  if("${isSystemDir}" STREQUAL "-1")
     set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  endif("${isSystemDir}" STREQUAL "-1")
endif()

configure_file(include/QBDI/Version.h.in "${CMAKE_SOURCE_DIR}/include/QBDI/Version.h" @ONLY)

# Add dependency build targets

add_subdirectory(deps)

if(${DEPENDENCY_SATISFIED})

# Add includes

set(LLVM_LIBS_DIR deps/llvm/${PLATFORM}/lib)

# Resolve LLVM options

if((${PLATFORM} STREQUAL "linux-X86_64") OR
   (${PLATFORM} STREQUAL "linux-X86") OR
   (${PLATFORM} STREQUAL "macOS-X86_64") OR
   (${PLATFORM} STREQUAL "macOS-X86") OR
   (${PLATFORM} STREQUAL "linux-ARM") OR
   (${PLATFORM} STREQUAL "iOS-ARM"))

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -Wall -ffunction-sections -fdata-sections -fvisibility=hidden -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -fvisibility-inlines-hidden  -fvisibility=hidden -std=c++11 -ffunction-sections -fdata-sections -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -fno-rtti")

elseif((${PLATFORM} STREQUAL "win-X86_64") OR
        (${PLATFORM} STREQUAL "win-X86"))

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /DWIN32 /D_WINDOWS /W3   /MP -D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_NONSTDC_NO_WARNINGS -D_SCL_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS -wd4141 -wd4146 -wd4180 -wd4244 -wd4258 -wd4267 -wd4291 -wd4345 -wd4351 -wd4355 -wd4456 -wd4457 -wd4458 -wd4459 -wd4503 -wd4624 -wd4722 -wd4800 -w14062 -we4238 -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DWIN32 /D_WINDOWS /W3     /MP -D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_NONSTDC_NO_WARNINGS -D_SCL_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS -wd4141 -wd4146 -wd4180 -wd4244 -wd4258 -wd4267 -wd4291 -wd4345 -wd4351 -wd4355 -wd4456 -wd4457 -wd4458 -wd4459 -wd4503 -wd4624 -wd4722 -wd4800 -w14062 -we4238 -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")

elseif((${PLATFORM} STREQUAL "android-ARM"))

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -ffunction-sections -fdata-sections -fvisibility=hidden")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fvisibility-inlines-hidden -fvisibility=hidden -std=c++11 -ffunction-sections -fdata-sections -fno-rtti")

endif()

# Force 32bit
if(${PLATFORM} STREQUAL "macOS-X86")
    set(CMAKE_C_FLAGS "-arch i386 ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "-arch i386 ${CMAKE_CXX_FLAGS}")
elseif(${PLATFORM} STREQUAL "linux-X86")
    set(CMAKE_C_FLAGS "-m32 ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "-m32 ${CMAKE_CXX_FLAGS}")
endif()

# Disable thumb on ARM (until we support it)
if(${ARCH} STREQUAL "ARM")
    # Some armv6 toolchains don't support this
    check_c_compiler_flag(-mno-thumb HAS_NO_THUMB)
    if(HAS_NO_THUMB)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mno-thumb")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mno-thumb")
    endif()
endif()

FUNCTION(PREPEND var prefix)
   SET(listVar "")
   FOREACH(f ${ARGN})
      LIST(APPEND listVar "${prefix}${f}")
   ENDFOREACH(f)
   SET(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(PREPEND)

# Generated using
# llvm-config --libs x86 mc mcparser mcdisassembler object support --system-libs
# TODO: use ExternalProject for llvm

# Common static LLVM objects
set(_LLVM_STATIC_LIBS LLVMBinaryFormat.a LLVMMCDisassembler.a LLVMSelectionDAG.a LLVMAsmPrinter.a LLVMCodeGen.a LLVMScalarOpts.a LLVMInstCombine.a LLVMTransformUtils.a LLVMAnalysis.a LLVMTarget.a LLVMObject.a LLVMMCParser.a LLVMBitReader.a LLVMCore.a LLVMMC.a LLVMSupport.a)

if(${PLATFORM} STREQUAL "linux-X86" OR ${PLATFORM} STREQUAL "linux-X86_64")
    find_package(Curses REQUIRED)
    set(NCURSES_LDFLAGS "-lncurses")
    if(CURSES_EXTRA_LIBRARY)
        set(NCURSES_LDFLAGS "${NCURSES_LDFLAGS} -ltinfo")
    endif()

    set(_LLVM_STATIC_LIBS ${_LLVM_STATIC_LIBS} LLVMProfileData.a LLVMX86Disassembler.a LLVMX86AsmParser.a LLVMX86CodeGen.a LLVMX86Desc.a LLVMX86Info.a LLVMX86AsmPrinter.a LLVMX86Utils.a)
    set(LLVM_LIBS -lrt -ldl ${NCURSES_LDFLAGS} -latomic -lpthread -lz -lm -lstdc++)

elseif((${PLATFORM} STREQUAL "win-X86_64") OR (${PLATFORM} STREQUAL "win-X86"))

    set(_LLVM_STATIC_LIBS LLVMAnalysis.lib LLVMBinaryFormat.lib LLVMCore.lib LLVMMCDisassembler.lib LLVMScalarOpts.lib LLVMTransformUtils.lib LLVMX86Desc.lib LLVMAsmPrinter.lib LLVMInstCombine.lib LLVMMCParser.lib LLVMSelectionDAG.lib LLVMX86AsmParser.lib LLVMX86Disassembler.lib LLVMBitReader.lib LLVMObject.lib LLVMSupport.lib LLVMX86AsmPrinter.lib LLVMX86Info.lib LLVMCodeGen.lib LLVMMC.lib LLVMProfileData.lib LLVMTarget.lib LLVMX86CodeGen.lib LLVMX86Utils.lib)
    set(LLVM_LIBS "")

elseif(${PLATFORM} STREQUAL "linux-ARM")

    set(_LLVM_STATIC_LIBS ${_LLVM_STATIC_LIBS} LLVMProfileData.a LLVMARMDisassembler.a LLVMARMCodeGen.a LLVMARMAsmParser.a LLVMARMDesc.a LLVMARMInfo.a LLVMARMAsmPrinter.a LLVMARMUtils.a)
    set(LLVM_LIBS -ldl -lpthread -lm -lstdc++)

elseif(${PLATFORM} STREQUAL "macOS-X86" OR ${PLATFORM} STREQUAL "macOS-X86_64")

    set(_LLVM_STATIC_LIBS ${_LLVM_STATIC_LIBS} LLVMDemangle.a LLVMX86Disassembler.a LLVMX86AsmParser.a LLVMX86CodeGen.a LLVMX86Desc.a LLVMX86Info.a LLVMX86AsmPrinter.a LLVMX86Utils.a)
    set(LLVM_LIBS -lz -lpthread -lffi -ledit -lcurses -lm -lc++)

elseif(${PLATFORM} STREQUAL "android-ARM")

    set(_LLVM_STATIC_LIBS ${_LLVM_STATIC_LIBS} LLVMProfileData.a LLVMARMDisassembler.a LLVMARMCodeGen.a LLVMARMAsmParser.a LLVMARMDesc.a LLVMARMInfo.a LLVMARMAsmPrinter.a LLVMARMUtils.a)
    set(LLVM_LIBS -ldl -lz -lm) # -lgnustl_static)

elseif(${PLATFORM} STREQUAL "iOS-ARM")

    set(_LLVM_STATIC_LIBS ${_LLVM_STATIC_LIBS} LLVMDemangle.a LLVMProfileData.a LLVMARMDisassembler.a LLVMARMCodeGen.a LLVMARMAsmParser.a LLVMARMDesc.a LLVMARMInfo.a LLVMARMAsmPrinter.a LLVMARMUtils.a)
    set(LLVM_LIBS -lz -lpthread -ledit -lcurses -lm -lc++)

endif()

# Add QBDI target

set(SOURCES
    "src/Engine/Engine.cpp"
    "src/Engine/VM.cpp"
    "src/Engine/VM_C.cpp"
    "src/ExecBlock/ExecBlock.cpp"
    "src/ExecBlock/ExecBlockManager.cpp"
    "src/ExecBroker/ExecBroker.cpp"
    "src/Patch/InstrRule.cpp"
    "src/Patch/InstrRules.cpp"
    "src/Patch/InstTransform.cpp"
    "src/Patch/PatchUtils.cpp"
    "src/Patch/${BASE_ARCH}/InstInfo_${BASE_ARCH}.cpp"
    "src/Patch/${BASE_ARCH}/PatchRules_${BASE_ARCH}.cpp"
    "src/Patch/${BASE_ARCH}/Layer2_${BASE_ARCH}.cpp"
    "src/Patch/${BASE_ARCH}/InstrRules_${BASE_ARCH}.cpp"
    "src/Patch/${BASE_ARCH}/PatchGenerator_${BASE_ARCH}.cpp"
    "src/Utility/memory_ostream.cpp"
    "src/Utility/Assembly.cpp"
    "src/Utility/Memory.cpp"
    "src/Utility/Memory_${OS}.cpp"
    "src/Utility/System.cpp"
    "src/Utility/LogSys.cpp"
    "src/Utility/Version.cpp"
    "src/Utility/String.cpp"
)

if(${OS} STREQUAL "iOS")
    set(SOURCES
        ${SOURCES}
        "src/Utility/System_iOS.cpp"
        "src/Utility/server-iOS-jit-user.c"
    )
else()
    set(SOURCES
        ${SOURCES}
        "src/Utility/System_generic.cpp"
    )
endif()

# Assembly stubs

if(${OS} STREQUAL "win")
    enable_language(ASM_MASM)
    set(ASM_EXT "asm")
else()
    set(CMAKE_ASM-ATT_COMPILER ${AS_BINARY})
    enable_language(ASM-ATT)
    set(ASM_EXT "s")
    if(${OS} STREQUAL "iOS")
        set(CMAKE_ASM-ATT_FLAGS "-arch ${ARM_ARCH}")
    elseif(${OS} STREQUAL "macOS" AND ${ARCH} STREQUAL "X86")
        set(CMAKE_ASM-ATT_FLAGS "-arch i386")
    elseif(${OS} STREQUAL "linux" AND ${ARCH} STREQUAL "X86")
        set(CMAKE_ASM-ATT_FLAGS "--32")
    endif()
endif()

set(SOURCES ${SOURCES} "src/ExecBlock/${PLATFORM}_Stub.${ASM_EXT}")

if (${PLATFORM} STREQUAL "win-X86")
    set_source_files_properties("src/ExecBlock/${PLATFORM}_Stub.${ASM_EXT}" PROPERTIES COMPILE_FLAGS "/safeseh")
endif()


# Create a list of static LLVM objects

if(${OS} STREQUAL "win")
    PREPEND(LLVM_STATIC_LIBS ${CMAKE_SOURCE_DIR}/${LLVM_LIBS_DIR}/ ${_LLVM_STATIC_LIBS})
else()
    PREPEND(LLVM_STATIC_LIBS ${CMAKE_SOURCE_DIR}/${LLVM_LIBS_DIR}/lib ${_LLVM_STATIC_LIBS})
endif()

# Final QBDI static library is a merge of static libraries
add_library(QBDI_obj STATIC ${SOURCES})

target_include_directories(QBDI_obj PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include/QBDI"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/llvm/${PLATFORM}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/llvm/${PLATFORM}/lib/Target/${LLVM_ARCH}"
)

# Hack because generators expressions don't work properly under visual studio ...
if(${OS} STREQUAL "win")
    set(qbdi_static_libs ${LLVM_STATIC_LIBS} ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/QBDI_obj.lib)
else()
    set(qbdi_static_libs ${LLVM_STATIC_LIBS} $<TARGET_FILE:QBDI_obj>)
endif()
MERGE_STATIC_LIBS(QBDI_static QBDI \${qbdi_static_libs})
add_library(QBDI::QBDI_static ALIAS QBDI_static)
target_include_directories(QBDI_static PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(QBDI_static PUBLIC ${LLVM_LIBS})
add_dependencies(QBDI_static QBDI_obj)

# QBDI as a shared library
add_library(QBDI SHARED cmake/stub.cpp)
add_library(QBDI::QBDI ALIAS QBDI)
target_include_directories(QBDI PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(QBDI PUBLIC ${LLVM_LIBS} PRIVATE QBDI_static)
add_dependencies(QBDI QBDI_obj)
set_target_properties(QBDI PROPERTIES OUTPUT_NAME QBDI)
# Hack: output stub not in the same directory than static to avoid conflicts
if(${OS} STREQUAL "win")
    set_target_properties(QBDI_static PROPERTIES OUTPUT_NAME QBDI_static)
endif()

if(DEFINED STRIP_PATH)
    # Force a strip cmd
    set(CMAKE_STRIP ${STRIP_PATH})
endif()
if(DEFINED CMAKE_STRIP AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(STRIP_ARGS "")
    if(APPLE)
        set(STRIP_ARGS ${STRIP_ARGS} -un)
    elseif(UNIX)
        set(STRIP_ARGS ${STRIP_ARGS} -x)
    endif()
    add_custom_command(TARGET QBDI
        POST_BUILD
        COMMAND ${CMAKE_STRIP} ${STRIP_ARGS} "$<TARGET_FILE:QBDI>")
endif()

# Configure QBDIPreload
if((${OS} STREQUAL "linux") OR (${OS} STREQUAL "android") OR (${OS} STREQUAL "macOS"))
    set(HAS_QBDIPRELOAD true)
else()
    set(HAS_QBDIPRELOAD false)
endif()
if(WIN32)
    set(HAS_QBDIPRELOAD_WIN true)
endif()

# Configure install
if(UNIX AND NOT ("${PLATFORM}" STREQUAL "android-ARM" OR "${PLATFORM}" STREQUAL "iOS-ARM"))
    set(FULL_PACKAGE true)
else()
    set(FULL_PACKAGE false)
endif()

# Install targets
set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "QBDI")
install(TARGETS QBDI_static
        EXPORT QBDI_targets
        ARCHIVE DESTINATION lib)

install(TARGETS QBDI
        EXPORT QBDI_targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION lib)

if(${OS} STREQUAL "win")
    install(FILES ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/QBDI.exp
            DESTINATION lib)
endif()

install(DIRECTORY include/QBDI DESTINATION include/
        PATTERN "*.in" EXCLUDE)

install(FILES include/QBDI.h DESTINATION include/)


if(FULL_PACKAGE)
    string(REPLACE ";" " " LLVM_LIBS_STR "${LLVM_LIBS}")
    configure_file(package/qbdi.pc.in package/qbdi.pc @ONLY)
    install(FILES ${CMAKE_BINARY_DIR}/package/qbdi.pc DESTINATION lib/pkgconfig/)

    set(RESOURCES_PREFIX ${CMAKE_INSTALL_PREFIX}/share/qbdi)

    install(EXPORT QBDI_targets
        FILE QBDIConfig.cmake
        NAMESPACE QBDI::
        DESTINATION ${RESOURCES_PREFIX}/cmake)

    # QBDI template
    configure_file(package/qbdi-template.in package/qbdi-template)
    install(DIRECTORY templates/qbdi_template DESTINATION ${RESOURCES_PREFIX}
            COMPONENT QBDITemplate)
    install(PROGRAMS ${CMAKE_BINARY_DIR}/package/qbdi-template DESTINATION bin
            COMPONENT QBDITemplate)

    set(PRELOAD_RESOURCES_PREFIX ${CMAKE_INSTALL_PREFIX}/share/qbdipreload)
    # QBDI preload template
    if(HAS_QBDIPRELOAD AND TOOLS_QBDIPRELOAD)
        configure_file(package/qbdi-preload-template.in package/qbdi-preload-template)
        install(DIRECTORY templates/qbdi_preload_template DESTINATION ${PRELOAD_RESOURCES_PREFIX}
                COMPONENT QBDITemplate)
        install(PROGRAMS ${CMAKE_BINARY_DIR}/package/qbdi-preload-template DESTINATION bin
                COMPONENT QBDITemplate)
    endif()

    # Frida template
    if(TOOLS_FRIDAQBDI)
        configure_file(package/qbdi-frida-template.in package/qbdi-frida-template)
        install(DIRECTORY templates/qbdi_frida_template DESTINATION ${RESOURCES_PREFIX}
                COMPONENT QBDIFridaTemplate)
        install(PROGRAMS ${CMAKE_BINARY_DIR}/package/qbdi-frida-template DESTINATION bin
                COMPONENT QBDIFridaTemplate)
        install(FILES tools/frida-qbdi.js DESTINATION ${RESOURCES_PREFIX})
    endif()

else()
    install(EXPORT QBDI_targets
        FILE QBDIConfig.cmake
        NAMESPACE QBDI::
        DESTINATION cmake)

    install(DIRECTORY templates/qbdi_template/ DESTINATION templates/qbdi_template)

    if(HAS_QBDIPRELOAD AND TOOLS_QBDIPRELOAD)
        install(DIRECTORY templates/qbdi_preload_template DESTINATION templates/qbdi_preload_template)
    endif()

    if(TOOLS_FRIDAQBDI)
        install(DIRECTORY templates/qbdi_frida_template DESTINATION templates/qbdi_frida_template)
    endif()

    install(FILES LICENSE.txt package/Readme.txt DESTINATION .)

    set(PRELOAD_RESOURCES_PREFIX qbdipreload)

    if(TOOLS_FRIDAQBDI)
        install(FILES tools/frida-qbdi.js DESTINATION .)
    endif()
endif()

# Configure iOS code signature
if(${OS} STREQUAL "iOS")
    # Find codesign executable
    execute_process(
        COMMAND xcrun -f codesign
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE CODESIGN
    )
    # Define a code signature macro
    macro(ADD_SIGNATURE target_name)
        add_custom_command(TARGET ${target_name}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E env ${CODESIGN} -f -s - $<TARGET_FILE:${target_name}>
        )
    endmacro()
else()
    # Define a dummy code signature macro
    macro(ADD_SIGNATURE target_name)
    endmacro()
endif()


# Add tests
if(TEST_QBDI)
    message(STATUS "Compile Test")
    add_subdirectory(test)
endif()

# Add tools
add_subdirectory(tools)

# Add examples
if(EXAMPLES)
    message(STATUS "Compile Exemples")
    add_subdirectory(examples)
endif()

# DEPENDENCY_SATISFIED
endif()

# Add documentation

add_subdirectory(docs)

# Add packaging

add_subdirectory(package)
