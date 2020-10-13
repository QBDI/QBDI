cmake_minimum_required (VERSION 3.5)
project(QBDI)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# Config platform & arch
include(QBDIConfig)
# Option
include(QBDIOptions)

# Force find system utils (strip)
include(CMakeFindBinUtils)

# Macros
include(cmake/merge_archives.cmake)

# CCACHE
# ======
if (QBDI_CCACHE)
    find_program(CCACHE_FOUND ccache)
    if (CCACHE_FOUND)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_FOUND})
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK    ${CCACHE_FOUND})
        message(STATUS "found CCACHE at ${CCACHE_FOUND}")
    else()
        # fallback to sccache (ccache doesn't work with MSVC
        find_program(SCCACHE_FOUND sccache)
        if (SCCACHE_FOUND)
            set(CMAKE_C_COMPILER_LAUNCHER ${SCCACHE_FOUND})
            set(CMAKE_CXX_COMPILER_LAUNCHER ${SCCACHE_FOUND})
            message(STATUS "found SCCACHE at ${SCCACHE_FOUND}")
        else()
            set(QBDI_CCACHE OFF)
            message(WARNING "CCACHE or SCCACHE not found: disabled")
        endif()
    endif()
endif()

# DEBUG log
if(QBDI_LOG_DEBUG)
    add_definitions(-D_QBDI_LOG_DEBUG)
endif()

# AVX
if(QBDI_DISABLE_AVX)
    add_definitions(-D_QBDI_FORCE_DISABLE_AVX)
endif()

include(CheckCCompilerFlag)

if (QBDI_ASAN)
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

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/QBDI/Version.h.in" "${CMAKE_CURRENT_BINARY_DIR}/include/QBDI/Version.h" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/QBDI/Config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/include/QBDI/Config.h" @ONLY)

# Add dependency build targets

add_subdirectory(deps)

if(${DEPENDENCY_SATISFIED})

    # Resolve LLVM options

    if( (QBDI_PLATFORM_LINUX    AND (QBDI_ARCH_X86 OR QBDI_ARCH_X86_64 OR QBDI_ARCH_ARM)) OR
        (QBDI_PLATFORM_OSX      AND (QBDI_ARCH_X86 OR QBDI_ARCH_X86_64)) OR
        (QBDI_PLATFORM_IOS      AND  QBDI_ARCH_ARM))

        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -Wall -ffunction-sections -fdata-sections -fvisibility=hidden -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -fvisibility-inlines-hidden  -fvisibility=hidden -ffunction-sections -fdata-sections -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -fno-rtti")

    elseif(QBDI_PLATFORM_WINDOWS AND (QBDI_ARCH_X86 OR QBDI_ARCH_X86_64))

        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /DWIN32 /D_WINDOWS /W3 /MP /permissive- -D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_NONSTDC_NO_WARNINGS -D_SCL_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS -wd4141 -wd4146 -wd4180 -wd4244 -wd4258 -wd4267 -wd4291 -wd4345 -wd4351 -wd4355 -wd4456 -wd4457 -wd4458 -wd4459 -wd4503 -wd4624 -wd4722 -wd4800 -w14062 -we4238 -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DWIN32 /D_WINDOWS /W3 /MP /permissive- -D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_NONSTDC_NO_WARNINGS -D_SCL_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS -wd4141 -wd4146 -wd4180 -wd4244 -wd4258 -wd4267 -wd4291 -wd4345 -wd4351 -wd4355 -wd4456 -wd4457 -wd4458 -wd4459 -wd4503 -wd4624 -wd4722 -wd4800 -w14062 -we4238 -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")

    elseif(QBDI_PLATFORM_ANDROID)

        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -Wall -ffunction-sections -fdata-sections -fvisibility=hidden")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -fvisibility-inlines-hidden -fvisibility=hidden -ffunction-sections -fdata-sections -fno-rtti")

    endif()

    # Setup assembly compile
    if(QBDI_PLATFORM_WINDOWS)
        enable_language(ASM_MASM)
        set(ASM_EXT "asm")
    else()
        set(CMAKE_ASM-ATT_COMPILER ${AS_BINARY})
        enable_language(ASM-ATT)
        set(ASM_EXT "s")
        if(QBDI_PLATFORM_IOS)
            set(CMAKE_ASM-ATT_FLAGS "-arch ${ARM_ARCH}")
        elseif(QBDI_PLATFORM_OSX AND QBDI_ARCH_X86)
            set(CMAKE_ASM-ATT_FLAGS "-arch i386")
        elseif(QBDI_PLATFORM_LINUX AND QBDI_ARCH_X86)
            set(CMAKE_ASM-ATT_FLAGS "--32")
        endif()
    endif()

    # Force 32bit
    if(QBDI_PLATFORM_OSX AND QBDI_ARCH_X86)
        set(CMAKE_C_FLAGS "-arch i386 ${CMAKE_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "-arch i386 ${CMAKE_CXX_FLAGS}")
    elseif(QBDI_PLATFORM_LINUX AND QBDI_ARCH_X86)
        set(CMAKE_C_FLAGS "-m32 ${CMAKE_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "-m32 ${CMAKE_CXX_FLAGS}")
    endif()

    # Disable thumb on ARM (until we support it)
    if(QBDI_ARCH_ARM)
        # Some armv6 toolchains don't support this
        check_c_compiler_flag(-mno-thumb HAS_NO_THUMB)
        if(HAS_NO_THUMB)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mno-thumb")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mno-thumb")
        endif()
    endif()

    FUNCTION(PREPEND var prefix suffix)
        SET(listVar "")
        FOREACH(f ${ARGN})
            LIST(APPEND listVar "${prefix}${f}${suffix}")
        ENDFOREACH(f)
        SET(${var} "${listVar}" PARENT_SCOPE)
    ENDFUNCTION(PREPEND)

    # Generated using
    # llvm-config --libs x86 mc mcparser mcdisassembler object support --system-libs
    # TODO: use ExternalProject for llvm

    # Common static LLVM objects
    set(_LLVM_STATIC_LIBS
      LLVMBinaryFormat
      LLVMMCDisassembler
      LLVMMCParser
      LLVMMC
      LLVMSupport
      LLVM${QBDI_LLVM_ARCH}Disassembler
      LLVM${QBDI_LLVM_ARCH}AsmParser
      LLVM${QBDI_LLVM_ARCH}Desc
      LLVM${QBDI_LLVM_ARCH}Info
      LLVM${QBDI_LLVM_ARCH}Utils)

    if(QBDI_PLATFORM_LINUX AND (QBDI_ARCH_X86 OR QBDI_ARCH_X86_64))

        set(LLVM_LIBS -lrt -ldl -lpthread -lz -lm -lstdc++)

    elseif(QBDI_PLATFORM_WINDOWS AND (QBDI_ARCH_X86 OR QBDI_ARCH_X86_64))

        set(LLVM_LIBS "")

    elseif(QBDI_PLATFORM_LINUX AND QBDI_ARCH_ARM)

        set(LLVM_LIBS -ldl -lpthread -lm -lstdc++)

    elseif(QBDI_PLATFORM_OSX AND (QBDI_ARCH_X86 OR QBDI_ARCH_X86_64))

        set(_LLVM_STATIC_LIBS ${_LLVM_STATIC_LIBS} LLVMDemangle)
        set(LLVM_LIBS -lz -lpthread -lffi -ledit -lm -lc++)

    elseif(QBDI_PLATFORM_ANDROID AND QBDI_ARCH_ARM)

        set(LLVM_LIBS -ldl -lz -lm) # -lgnustl_static)

    elseif(QBDI_PLATFORM_ANDROID AND (QBDI_ARCH_X86 OR QBDI_ARCH_X86_64))

        set(LLVM_LIBS -ldl -lz -lm) # -lgnustl_static)

    elseif(QBDI_PLATFORM_IOS AND QBDI_ARCH_ARM)

        set(_LLVM_STATIC_LIBS ${_LLVM_STATIC_LIBS} LLVMDemangle)
        set(LLVM_LIBS -lz -lpthread -ledit -lm -lc++)

    endif()

    set(LLVM_LIBS_DIR deps/llvm/${QBDI_PLATFORM}-${QBDI_ARCH}/lib)

    if(QBDI_PLATFORM_WINDOWS)
        PREPEND(LLVM_STATIC_LIBS ${CMAKE_SOURCE_DIR}/${LLVM_LIBS_DIR}/ ".lib" ${_LLVM_STATIC_LIBS})
    else()
        PREPEND(LLVM_STATIC_LIBS ${CMAKE_SOURCE_DIR}/${LLVM_LIBS_DIR}/lib ".a" ${_LLVM_STATIC_LIBS})
    endif()

    # Add QBDI source file in QBDI_obj
    add_library(QBDI_src INTERFACE)

    include("${CMAKE_CURRENT_SOURCE_DIR}/src/CMakeLists.txt")

    target_include_directories(QBDI_src INTERFACE
        "${CMAKE_CURRENT_BINARY_DIR}/include"
        "${CMAKE_CURRENT_BINARY_DIR}/include/QBDI"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/QBDI"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/deps/llvm/${QBDI_PLATFORM}-${QBDI_ARCH}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/deps/llvm/${QBDI_PLATFORM}-${QBDI_ARCH}/lib/Target/${QBDI_LLVM_ARCH}"
        )

    if (QBDI_STATIC_LIBRARY)
        add_library(QBDI_obj STATIC)
        target_link_libraries(QBDI_obj PRIVATE QBDI_src)

        set_target_properties(QBDI_obj
            PROPERTIES
                CXX_STANDARD 14
                CXX_STANDARD_REQUIRED ON
        )

        set(qbdi_static_libs ${LLVM_STATIC_LIBS} QBDI_obj)

        MERGE_STATIC_LIBS(QBDI_static QBDI \${qbdi_static_libs})

        add_library(QBDI::QBDI_static ALIAS QBDI_static)
        target_include_directories(QBDI_static PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/QBDI>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/QBDI>
            )

        target_link_libraries(QBDI_static PUBLIC ${LLVM_LIBS})
        add_dependencies(QBDI_static QBDI_obj)

        if(QBDI_PLATFORM_WINDOWS)
            set_target_properties(QBDI_static PROPERTIES OUTPUT_NAME QBDI_static)
        endif()

        install(TARGETS QBDI_static
            EXPORT QBDI_targets
            COMPONENT QBDI
            ARCHIVE DESTINATION lib)

    endif()

    # QBDI as a shared library
    if (QBDI_SHARED_LIBRARY)
        add_library(QBDI SHARED)
        add_library(QBDI::QBDI ALIAS QBDI)

        target_link_libraries(QBDI PRIVATE QBDI_src)
        target_link_libraries(QBDI PRIVATE ${LLVM_STATIC_LIBS})

        set_target_properties(QBDI
            PROPERTIES
                CXX_STANDARD 14
                CXX_STANDARD_REQUIRED ON
        )

        target_include_directories(QBDI PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/QBDI>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/QBDI>
            )

        target_link_libraries(QBDI PUBLIC ${LLVM_LIBS} PRIVATE ${LLVM_STATIC_LIBS})
        set_target_properties(QBDI PROPERTIES OUTPUT_NAME QBDI)

        if(DEFINED STRIP_PATH)
            # Force a strip cmd
            set(CMAKE_STRIP ${STRIP_PATH})
        endif()
        if (DEFINED CMAKE_STRIP AND
                NOT CMAKE_STRIP STREQUAL "CMAKE_STRIP-NOTFOUND" AND
                NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND
                NOT QBDI_ENABLE_LOG_DEBUG)

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

        # Install QBDI targets
        install(TARGETS QBDI
            EXPORT QBDI_targets
            COMPONENT QBDI
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
            RUNTIME DESTINATION lib)

        if(QBDI_PLATFORM_WINDOWS)
            install(FILES ${CMAKE_BINARY_DIR}/QBDI.exp
                COMPONENT QBDI
                DESTINATION lib)
        endif()

    endif()


    set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "QBDI")
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/QBDI" DESTINATION include/ PATTERN "*.in" EXCLUDE)
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/QBDI" DESTINATION include/)

    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/QBDI.h" DESTINATION include/)

    # Configure install
    if(UNIX AND NOT (QBDI_ARCH_ARM AND (QBDI_PLATFORM_IOS OR QBDI_PLATFORM_ANDROID)))
        set(FULL_PACKAGE true)
    else()
        set(FULL_PACKAGE false)
    endif()

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
        if(QBDI_TOOLS_QBDIPRELOAD)
            configure_file(package/qbdi-preload-template.in package/qbdi-preload-template)
            install(DIRECTORY templates/qbdi_preload_template DESTINATION ${PRELOAD_RESOURCES_PREFIX}
                COMPONENT QBDITemplate)
            install(PROGRAMS ${CMAKE_BINARY_DIR}/package/qbdi-preload-template DESTINATION bin
                COMPONENT QBDITemplate)
        endif()

        # Frida template
        if(QBDI_TOOLS_FRIDAQBDI)
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

        if(QBDI_TOOLS_QBDIPRELOAD)
            install(DIRECTORY templates/qbdi_preload_template DESTINATION templates/qbdi_preload_template)
        endif()

        if(QBDI_TOOLS_FRIDAQBDI)
            install(DIRECTORY templates/qbdi_frida_template DESTINATION templates/qbdi_frida_template)
            install(FILES tools/frida-qbdi.js DESTINATION .)
        endif()

        install(FILES LICENSE.txt package/Readme.txt DESTINATION .)

        set(PRELOAD_RESOURCES_PREFIX qbdipreload)
    endif()

    # Configure iOS code signature
    if(QBDI_PLATFORM_IOS)
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
    if(QBDI_TEST OR QBDI_BENCHMARK)
        add_subdirectory(test)
    endif()

    # Add tools
    add_subdirectory(tools)

    # Add examples
    if(QBDI_EXAMPLES)
        add_subdirectory(examples)
    endif()

    # DEPENDENCY_SATISFIED
endif()

# Add documentation

add_subdirectory(docs)

# Add packaging

add_subdirectory(package)
