if(__add_qbdi_config)
  return()
endif()
set(__add_qbdi_config ON)

# ========================
# CMAKE build type
# ========================

set(DEFAULT_BUILD_TYPE Release)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(
    STATUS
      "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE
      "${DEFAULT_BUILD_TYPE}"
      CACHE STRING "Used default build type (${DEFAULT_BUILD_TYPE})." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Release" "Debug"
                                               "MinSizeRel" "RelWithDebInfo")
endif()

# ========================
# QBDI Version
# ========================
# note: the version should also be changed in the followed files:
# - docker/archlinux/PKGBUILD.{X86|X86_64}
# - docker/common.sh
# - setup.cfg
# - tools/frida-qbdi.js
set(QBDI_VERSION_MAJOR 0)
set(QBDI_VERSION_MINOR 12)
set(QBDI_VERSION_PATCH 1)
set(QBDI_VERSION_DEV 0)

set(QBDI_VERSION_STRING
    "${QBDI_VERSION_MAJOR}.${QBDI_VERSION_MINOR}.${QBDI_VERSION_PATCH}")
if(QBDI_VERSION_DEV)
  set(QBDI_VERSION_STRING "${QBDI_VERSION_STRING}-devel")
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git"
     AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    set_property(
      DIRECTORY
      APPEND
      PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    find_package(Git REQUIRED)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      OUTPUT_VARIABLE COMMIT_HASH
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(QBDI_VERSION_FULL_STRING "${QBDI_VERSION_STRING}-${COMMIT_HASH}")
  elseif(DEFINED ENV{APPVEYOR_REPO_COMMIT})
    set(QBDI_VERSION_FULL_STRING
        "${QBDI_VERSION_STRING}-$ENV{APPVEYOR_REPO_COMMIT}")
  else()
    set(QBDI_VERSION_FULL_STRING "${QBDI_VERSION_STRING}")
  endif()
else()
  set(QBDI_VERSION_FULL_STRING "${QBDI_VERSION_STRING}")
endif()

# ========================
# QBDI Platform
# ========================
set(QBDI_SUPPORTED_PLATFORMS "android" "linux" "windows" "osx" "ios")

set(QBDI_SUPPORTED_ARCH "ARM" "AARCH64" "X86" "X86_64")

set(QBDI_PLATFORM_WINDOWS 0)
set(QBDI_PLATFORM_LINUX 0)
set(QBDI_PLATFORM_ANDROID 0)
set(QBDI_PLATFORM_OSX 0)
set(QBDI_PLATFORM_IOS 0)

set(QBDI_ARCH_X86 0)
set(QBDI_ARCH_X86_64 0)
set(QBDI_ARCH_ARM 0)
set(QBDI_ARCH_AARCH64 0)

set(QBDI_BITS_32 0)
set(QBDI_BITS_64 0)

# Check platform
if(NOT DEFINED QBDI_PLATFORM)
  message(
    FATAL_ERROR
      "Please set 'QBDI_PLATFORM'\nSupported platform: ${QBDI_SUPPORTED_PLATFORMS}"
  )
elseif(QBDI_PLATFORM STREQUAL "windows")
  set(QBDI_PLATFORM_WINDOWS 1)
elseif(QBDI_PLATFORM STREQUAL "linux")
  set(QBDI_PLATFORM_LINUX 1)
elseif(QBDI_PLATFORM STREQUAL "android")
  set(QBDI_PLATFORM_ANDROID 1)
elseif(QBDI_PLATFORM STREQUAL "osx")
  set(QBDI_PLATFORM_OSX 1)
elseif(QBDI_PLATFORM STREQUAL "ios")
  set(QBDI_PLATFORM_IOS 1)
else()
  message(
    FATAL_ERROR
      "${QBDI_PLATFORM} is not a supported platform.\nCurrently supported: ${QBDI_SUPPORTED_PLATFORMS}"
  )
endif()

# Check arch
if(NOT DEFINED QBDI_ARCH)
  message(
    FATAL_ERROR
      "Please set 'QBDI_ARCH'\nSupported architecture: ${QBDI_SUPPORTED_ARCH}")
elseif(QBDI_ARCH STREQUAL "ARM")
  set(QBDI_ARCH_ARM 1)
  set(QBDI_BITS_32 1)
  set(QBDI_LLVM_ARCH "ARM")
  set(QBDI_BASE_ARCH "ARM")
elseif(QBDI_ARCH STREQUAL "AARCH64")
  set(QBDI_ARCH_AARCH64 1)
  set(QBDI_BITS_64 1)
  set(QBDI_LLVM_ARCH "AArch64")
  set(QBDI_BASE_ARCH "AARCH64")
elseif(QBDI_ARCH STREQUAL "X86")
  set(QBDI_ARCH_X86 1)
  set(QBDI_BITS_32 1)
  set(QBDI_LLVM_ARCH "X86")
  set(QBDI_BASE_ARCH "X86_64")
elseif(QBDI_ARCH STREQUAL "X86_64")
  set(QBDI_ARCH_X86_64 1)
  set(QBDI_BITS_64 1)
  set(QBDI_LLVM_ARCH "X86")
  set(QBDI_BASE_ARCH "X86_64")
else()
  message(
    FATAL_ERROR
      "${QBDI_ARCH} is not a supported architecture.\nCurrently supported: ${QBDI_SUPPORTED_ARCH}"
  )
endif()

if(QBDI_PLATFORM_WINDOWS AND NOT (QBDI_ARCH_X86 OR QBDI_ARCH_X86_64))
  message(
    FATAL_ERROR
      "Windows platform is only supported for X86 and X86_64 architectures.")
endif()

if(QBDI_PLATFORM_OSX
   AND NOT
       (QBDI_ARCH_X86
        OR QBDI_ARCH_X86_64
        OR QBDI_ARCH_AARCH64))
  message(
    FATAL_ERROR
      "OSX platform is only supported for X86 and X86_64 architectures.")
endif()

if(QBDI_PLATFORM_IOS AND NOT (QBDI_ARCH_ARM OR QBDI_ARCH_AARCH64))
  message(
    FATAL_ERROR
      "IOS platform is only supported for ARM and AARCH64 architecture.")
endif()

if(QBDI_PLATFORM_OSX)
  if(QBDI_ARCH_X86_64 AND NOT ("${CMAKE_HOST_SYSTEM_PROCESSOR}" STREQUAL
                               "x86_64"))
    set(CMAKE_OSX_ARCHITECTURES
        "x86_64"
        CACHE STRING "" FORCE)
    set(CMAKE_CROSSCOMPILING
        "TRUE"
        CACHE STRING "" FORCE)
  elseif(QBDI_ARCH_AARCH64 AND NOT ("${CMAKE_HOST_SYSTEM_PROCESSOR}" STREQUAL
                                    "arm64"))
    set(CMAKE_OSX_ARCHITECTURES
        "arm64"
        CACHE STRING "" FORCE)
    set(CMAKE_CROSSCOMPILING
        "TRUE"
        CACHE STRING "" FORCE)
  endif()
endif()

if(QBDI_PTRAUTH AND NOT ((QBDI_PLATFORM_OSX OR QBDI_PLATFORM_IOS)
                         AND QBDI_ARCH_AARCH64))
  message(
    FATAL_ERROR
      "PTRAUTH is only supported for IOS and OSX platforms and AARCH64 architecture."
  )
endif()

message(STATUS "")
message(STATUS "== QBDI Target ==")
message(STATUS "QBDI Platform:      ${QBDI_PLATFORM}")
message(STATUS "QBDI ARCH:          ${QBDI_ARCH}")
message(STATUS "LLVM ARCH:          ${QBDI_LLVM_ARCH}")
message(STATUS "Version:            ${QBDI_VERSION_FULL_STRING}")
message(STATUS "")
