cmake_minimum_required(VERSION 3.12)
project(QBDI)

include(GNUInstallDirs)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/"
     "${CMAKE_CURRENT_SOURCE_DIR}/cmake/llvm")

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
if(QBDI_CCACHE)
  find_program(CCACHE_FOUND ccache)
  if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_FOUND})
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE_FOUND})
    message(STATUS "found CCACHE at ${CCACHE_FOUND}")
  else()
    # fallback to sccache (ccache doesn't work with MSVC
    find_program(SCCACHE_FOUND sccache)
    if(SCCACHE_FOUND)
      set(CMAKE_C_COMPILER_LAUNCHER ${SCCACHE_FOUND})
      set(CMAKE_CXX_COMPILER_LAUNCHER ${SCCACHE_FOUND})
      message(STATUS "found SCCACHE at ${SCCACHE_FOUND}")
    else()
      set(QBDI_CCACHE OFF)
      message(WARNING "CCACHE or SCCACHE not found: disabled")
    endif()
  endif()
endif()

# AVX
if(QBDI_DISABLE_AVX)
  add_definitions(-D_QBDI_FORCE_DISABLE_AVX)
endif()

include(CheckCCompilerFlag)

if(QBDI_ASAN)
  set(CMAKE_REQUIRED_FLAGS "-Werror -fsanitize=address")
  check_c_compiler_flag("-fsanitize=address" HAVE_FLAG_SANITIZE_ADDRESS)
  unset(CMAKE_REQUIRED_FLAGS)
  if(HAVE_FLAG_SANITIZE_ADDRESS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    message(STATUS "ASAN is activated")
  else()
    message(WARNING "ASAN can't be activated")
  endif()
endif()

# Configure RPATH on OS X

if(APPLE)
  set(CMAKE_MACOSX_RPATH TRUE)

  # use, i.e. don't skip the full RPATH for the build tree
  set(CMAKE_SKIP_BUILD_RPATH FALSE)

  # when building, don't use the install RPATH already
  # (but later on when installing)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

  # add the automatically determined parts of the RPATH
  # which point to directories outside the build tree to the install RPATH
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

  # the RPATH to be used when installing, but only if it's not a system directory
  list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES
       "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
  if("${isSystemDir}" STREQUAL "-1")
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  endif("${isSystemDir}" STREQUAL "-1")
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/QBDI/Version.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/include/QBDI/Version.h" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/QBDI/Config.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/include/QBDI/Config.h" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/Utility/Version_commit.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/src/Utility/Version_commit.h" @ONLY)

# log debug
set(QBDI_COMMON_DEFINITION)
if(QBDI_LOG_DEBUG)
  message(STATUS "Compiling with QBDI_LOG_DEBUG")
  set(QBDI_COMMON_DEFINITION ${QBDI_COMMON_DEFINITION} -D_QBDI_LOG_DEBUG)
endif()

if(QBDI_PLATFORM_WINDOWS)
  set(QBDI_COMMON_C_FLAGS
      /DWIN32
      /D_WINDOWS
      /W3
      /MP
      /permissive-
      -D_CRT_SECURE_NO_DEPRECATE
      -D_CRT_SECURE_NO_WARNINGS
      -D_CRT_NONSTDC_NO_DEPRECATE
      -D_CRT_NONSTDC_NO_WARNINGS
      -D_SCL_SECURE_NO_DEPRECATE
      -D_SCL_SECURE_NO_WARNINGS
      -wd4141
      -wd4146
      -wd4180
      -wd4244
      -wd4258
      -wd4267
      -wd4291
      -wd4345
      -wd4351
      -wd4355
      -wd4456
      -wd4457
      -wd4458
      -wd4459
      -wd4503
      -wd4624
      -wd4722
      -wd4800
      -w14062
      -we4238
      -D__STDC_CONSTANT_MACROS
      -D__STDC_FORMAT_MACROS
      -D__STDC_LIMIT_MACROS)
  set(QBDI_COMMON_CXX_FLAGS
      /DWIN32
      /D_WINDOWS
      /W3
      /MP
      /permissive-
      -D_CRT_SECURE_NO_DEPRECATE
      -D_CRT_SECURE_NO_WARNINGS
      -D_CRT_NONSTDC_NO_DEPRECATE
      -D_CRT_NONSTDC_NO_WARNINGS
      -D_SCL_SECURE_NO_DEPRECATE
      -D_SCL_SECURE_NO_WARNINGS
      -wd4141
      -wd4146
      -wd4180
      -wd4244
      -wd4258
      -wd4267
      -wd4291
      -wd4345
      -wd4351
      -wd4355
      -wd4456
      -wd4457
      -wd4458
      -wd4459
      -wd4503
      -wd4624
      -wd4722
      -wd4800
      -w14062
      -we4238
      -D__STDC_CONSTANT_MACROS
      -D__STDC_FORMAT_MACROS
      -D__STDC_LIMIT_MACROS)
  # set _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING for llvm compile
  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING"
  )
else()
  set(QBDI_COMMON_C_FLAGS -Wall -ffunction-sections -fdata-sections
                          -fvisibility-inlines-hidden -fvisibility=hidden -fPIC)
  set(QBDI_COMMON_CXX_FLAGS
      -Wall
      -ffunction-sections
      -fdata-sections
      -fvisibility-inlines-hidden
      -fvisibility=hidden
      -fPIC
      -fno-rtti)
  if(NOT QBDI_PLATFORM_ANDROID)
    set(QBDI_COMMON_DEFINITION
        ${QBDI_COMMON_DEFINITION} -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS
        -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS)
  endif()
endif()

# Add llvm
include(QBDI_llvm)

# DEBUG log (spdlog option)
include(spdlog)

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

# Add QBDI source file in QBDI_obj
add_library(QBDI_src INTERFACE)
add_library(QBDI_shared_src INTERFACE)

target_link_libraries(QBDI_src INTERFACE qbdi-llvm)
target_link_libraries(QBDI_src INTERFACE spdlog_header_only)

include("${CMAKE_CURRENT_SOURCE_DIR}/src/CMakeLists.txt")

target_compile_features(QBDI_src INTERFACE cxx_std_17)
target_compile_options(
  QBDI_src INTERFACE $<$<COMPILE_LANGUAGE:C>:${QBDI_COMMON_C_FLAGS}>)
target_compile_options(
  QBDI_src INTERFACE $<$<COMPILE_LANGUAGE:CXX>:${QBDI_COMMON_CXX_FLAGS}>)
target_compile_definitions(QBDI_src INTERFACE ${QBDI_COMMON_DEFINITION})
target_include_directories(
  QBDI_src
  INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_BINARY_DIR}/src")

target_include_directories(
  QBDI_shared_src INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/include"
                            "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(QBDI_STATIC_LIBRARY)
  add_library(QBDI_obj STATIC)
  target_link_libraries(QBDI_obj PRIVATE QBDI_src)
  set_target_properties(QBDI_obj PROPERTIES POSITION_INDEPENDENT_CODE ON)

  set(qbdi_static_libs qbdi-llvm QBDI_obj)

  merge_static_libs(QBDI_static QBDI \${qbdi_static_libs})

  add_library(QBDI::QBDI_static ALIAS QBDI_static)
  target_include_directories(
    QBDI_static
    PUBLIC $<INSTALL_INTERFACE:include>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)

  target_link_libraries(QBDI_static PUBLIC ${QBDI_LLVM_LINK_LIBRARY})
  if(QBDI_PLATFORM_ANDROID)
    target_link_libraries(QBDI_static PUBLIC log)
  endif()
  add_dependencies(QBDI_static QBDI_obj)

  if(QBDI_PLATFORM_WINDOWS)
    set_target_properties(QBDI_static PROPERTIES OUTPUT_NAME QBDI_static)
  endif()

  install(
    TARGETS QBDI_static
    EXPORT QBDI_targets
    COMPONENT QBDI
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_LIBDIR}")

endif()

# QBDI as a shared library
if(QBDI_SHARED_LIBRARY)
  add_library(QBDI SHARED)
  add_library(QBDI::QBDI ALIAS QBDI)

  target_include_directories(
    QBDI
    PUBLIC $<INSTALL_INTERFACE:include>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)

  target_link_libraries(QBDI PRIVATE QBDI_src QBDI_shared_src qbdi-llvm)

  set_target_properties(QBDI PROPERTIES OUTPUT_NAME QBDI)

  if(DEFINED STRIP_PATH)
    # Force a strip cmd
    set(CMAKE_STRIP ${STRIP_PATH})
  endif()
  if(DEFINED CMAKE_STRIP
     AND NOT CMAKE_STRIP STREQUAL "CMAKE_STRIP-NOTFOUND"
     AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug"
     AND NOT QBDI_ENABLE_LOG_DEBUG)

    set(STRIP_ARGS "")
    if(APPLE)
      set(STRIP_ARGS ${STRIP_ARGS} -un)
    elseif(UNIX)
      set(STRIP_ARGS ${STRIP_ARGS} -x)
    endif()
    add_custom_command(
      TARGET QBDI
      POST_BUILD
      COMMAND ${CMAKE_STRIP} ${STRIP_ARGS} "$<TARGET_FILE:QBDI>")
  endif()

  # Install QBDI targets
  install(
    TARGETS QBDI
    EXPORT QBDI_targets
    COMPONENT QBDI
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_LIBDIR}")

  if(QBDI_PLATFORM_WINDOWS)
    install(
      FILES ${CMAKE_BINARY_DIR}/QBDI.exp
      COMPONENT QBDI
      DESTINATION "${CMAKE_INSTALL_LIBDIR}")
  endif()

endif()

set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "QBDI")
install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/QBDI"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  PATTERN "*.in" EXCLUDE)
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/QBDI"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/QBDI.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

# Configure install
if(UNIX AND NOT (QBDI_ARCH_ARM AND (QBDI_PLATFORM_IOS OR QBDI_PLATFORM_ANDROID)
                ))
  set(FULL_PACKAGE true)
else()
  set(FULL_PACKAGE false)
endif()

if(FULL_PACKAGE)
  #string(REPLACE ";" " " LLVM_LIBS_STR "${LLVM_LIBS}")
  #configure_file("${CMAKE_CURRENT_SOURCE_DIR}/package/qbdi.pc.in"
  #               "${CMAKE_CURRENT_BINARY_DIR}/package/qbdi.pc" @ONLY)
  #install(FILES "${CMAKE_CURRENT_BINARY_DIR}/package/qbdi.pc"
  #        DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig/")

  set(RESOURCES_PREFIX "${CMAKE_INSTALL_DATADIR}/qbdi${QBDI_ARCH}")

  install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt"
    DESTINATION "${RESOURCES_PREFIX}"
    COMPONENT QBDI)

  install(
    EXPORT QBDI_targets
    FILE QBDIConfig.cmake
    NAMESPACE QBDI::
    DESTINATION ${RESOURCES_PREFIX}/cmake)

  install(
    EXPORT QBDI_targets
    FILE QBDI${QBDI_ARCH}Config.cmake
    NAMESPACE QBDI::${QBDI_ARCH}::
    DESTINATION ${RESOURCES_PREFIX}/cmake)

  # QBDI template
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/package/qbdi-template.in"
                 "${CMAKE_CURRENT_BINARY_DIR}/package/qbdi-template")
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/templates/qbdi_template/CMakeLists.txt.in"
    "${CMAKE_CURRENT_BINARY_DIR}/templates/qbdi_template/CMakeLists.txt" @ONLY)
  install(
    DIRECTORY templates/qbdi_template
    DESTINATION "${RESOURCES_PREFIX}"
    COMPONENT QBDITemplate
    PATTERN "CMakeLists.txt.in" EXCLUDE)
  install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/templates/qbdi_template/CMakeLists.txt"
    DESTINATION "${RESOURCES_PREFIX}/qbdi_template")
  install(
    PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/package/qbdi-template"
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
    COMPONENT QBDITemplate
    RENAME "qbdi-template-${QBDI_ARCH}")

  set(PRELOAD_RESOURCES_PREFIX
      "${CMAKE_INSTALL_DATADIR}/qbdipreload${QBDI_ARCH}")
  # QBDI preload template
  if(QBDI_TOOLS_QBDIPRELOAD)
    configure_file(
      "${CMAKE_CURRENT_SOURCE_DIR}/package/qbdi-preload-template.in"
      "${CMAKE_CURRENT_BINARY_DIR}/package/qbdi-preload-template")
    configure_file(
      "${CMAKE_CURRENT_SOURCE_DIR}/templates/qbdi_preload_template/CMakeLists.txt.in"
      "${CMAKE_CURRENT_BINARY_DIR}/templates/qbdi_preload_template/CMakeLists.txt"
      @ONLY)
    install(
      DIRECTORY templates/qbdi_preload_template
      DESTINATION "${PRELOAD_RESOURCES_PREFIX}"
      COMPONENT QBDITemplate
      PATTERN "CMakeLists.txt.in" EXCLUDE)
    install(
      FILES
        "${CMAKE_CURRENT_BINARY_DIR}/templates/qbdi_preload_template/CMakeLists.txt"
      DESTINATION "${PRELOAD_RESOURCES_PREFIX}/qbdi_preload_template")
    install(
      PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/package/qbdi-preload-template"
      DESTINATION "${CMAKE_INSTALL_BINDIR}"
      COMPONENT QBDITemplate
      RENAME "qbdi-preload-template-${QBDI_ARCH}")
  endif()

  # Frida template
  if(QBDI_TOOLS_FRIDAQBDI)
    configure_file(package/qbdi-frida-template.in package/qbdi-frida-template)
    install(
      DIRECTORY templates/qbdi_frida_template
      DESTINATION "${RESOURCES_PREFIX}"
      COMPONENT QBDIFridaTemplate)
    install(
      PROGRAMS ${CMAKE_BINARY_DIR}/package/qbdi-frida-template
      DESTINATION "${CMAKE_INSTALL_BINDIR}"
      COMPONENT QBDIFridaTemplate
      RENAME "qbdi-frida-template-${QBDI_ARCH}")
    install(FILES tools/frida-qbdi.js DESTINATION "${RESOURCES_PREFIX}")
  endif()

else()
  install(
    EXPORT QBDI_targets
    FILE QBDIConfig.cmake
    NAMESPACE QBDI::
    DESTINATION cmake)

  install(
    EXPORT QBDI_targets
    FILE QBDI${QBDI_ARCH}Config.cmake
    NAMESPACE QBDI::${QBDI_ARCH}::
    DESTINATION cmake)

  install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt" DESTINATION .)

  install(DIRECTORY templates/qbdi_template/
          DESTINATION templates/qbdi_template)

  if(QBDI_TOOLS_QBDIPRELOAD)
    install(DIRECTORY templates/qbdi_preload_template
            DESTINATION templates/qbdi_preload_template)
  endif()

  if(QBDI_TOOLS_FRIDAQBDI)
    install(DIRECTORY templates/qbdi_frida_template
            DESTINATION templates/qbdi_frida_template)
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
    OUTPUT_VARIABLE CODESIGN)
  # Define a code signature macro
  macro(ADD_SIGNATURE target_name)
    add_custom_command(
      TARGET ${target_name}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E env ${CODESIGN} -f -s -
              $<TARGET_FILE:${target_name}>)
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

# Add documentation

add_subdirectory(docs)

# Add packaging

add_subdirectory(package)
