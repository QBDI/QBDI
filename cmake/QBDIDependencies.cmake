if(__add_qbdi_deps)
  return()
endif()
set(__add_qbdi_deps ON)
include(FetchContent)

set(QBDI_THIRD_PARTY_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/third-party/")
set(qbdi_deps)

# Custom FetchContent Command
# ===========================

# since CMAKE 3.30, FetchContent_Declare doesn't support DOWNLOAD_DIR, which is
# used in order to cache the downloaded archive during CI.
# The followed function are used to keep this functionnality

function(DOWNLOAD_PACKAGE PACKAGE_NAME)

  string(TOLOWER "${PACKAGE_NAME}" PACKAGE_NAME_LOWER)
  set(FetchName "qbdi_deps_${PACKAGE_NAME_LOWER}")

  FetchContent_Populate(
    ${FetchName}
    DOWNLOAD_DIR "${QBDI_THIRD_PARTY_DIRECTORY}/${PACKAGE_NAME}_download"
    SOURCE_DIR "${FETCHCONTENT_BASE_DIR}/${PACKAGE_NAME_LOWER}-src"
    BINARY_DIR "${FETCHCONTENT_BASE_DIR}/${PACKAGE_NAME_LOWER}-build"
    SUBBUILD_DIR "${FETCHCONTENT_BASE_DIR}/${PACKAGE_NAME_LOWER}-subbuild"
    QUIET ${ARGN})

  set("${PACKAGE_NAME}_EXTRACT_PATH"
      "${${FetchName}_SOURCE_DIR}"
      PARENT_SCOPE)
endfunction()

function(PREPARE_PACKAGE PACKAGE_NAME)

  cmake_parse_arguments(ARG "" "URL;URL_HASH" "" ${ARGN})

  if(NOT ((DEFINED ARG_URL) AND (DEFINED ARG_URL_HASH)))
    message(FATAL_ERROR "missing URL or URL_HASH in function argument")
  endif()

  download_package(${PACKAGE_NAME} URL "${ARG_URL}" URL_HASH "${ARG_URL_HASH}")

  FetchContent_Declare(
    ${PACKAGE_NAME} SOURCE_DIR "${${PACKAGE_NAME}_EXTRACT_PATH}"
                    ${ARG_UNPARSED_ARGUMENTS})
endfunction()

# Spdlog
# ======
set(SPDLOG_VERSION 1.15.0)

prepare_package(
  spdlog
  URL
  "https://github.com/gabime/spdlog/archive/v${SPDLOG_VERSION}.zip"
  URL_HASH
  "SHA256=076f3b4d452b95433083bcc66d07f79addba2d3fcb2b9177abeb7367d47aefbb"
  EXCLUDE_FROM_ALL)

list(APPEND qbdi_deps spdlog)

set(SPDLOG_NO_EXCEPTIONS
    ON
    CACHE BOOL "Enable SPDLOG_NO_EXCEPTIONS")
set(SPDLOG_NO_TLS
    ON
    CACHE BOOL "Enable SPDLOG_NO_TLS")
set(SPDLOG_NO_THREAD_ID
    ON
    CACHE BOOL "Enable SPDLOG_NO_THREAD_ID")
if(QBDI_LOG_DEBUG)
  set(SPDLOG_ACTIVE_LEVEL
      SPDLOG_LEVEL_DEBUG
      CACHE BOOL "Enable SPDLOG_LEVEL_DEBUG level")
endif()

# Catch2
# ======
if((QBDI_TEST) OR (QBDI_BENCHMARK))
  prepare_package(
    Catch2
    URL
    "https://github.com/catchorg/Catch2/archive/refs/tags/v3.3.1.zip"
    URL_HASH
    "SHA256=5e5283bf93b2693f6877bba3eaa76d66588955374d0cec5b40117066c044ad5e"
    EXCLUDE_FROM_ALL)
  list(APPEND qbdi_deps Catch2)
endif()

# Pybind11
# ========
if(QBDI_TOOLS_PYQBDI)
  if(DEFINED QBDI_TOOLS_PYQBDI_TARGET_PYTHON_VERSION)
    find_package(
      Python3 "${QBDI_TOOLS_PYQBDI_TARGET_PYTHON_VERSION}" EXACT REQUIRED
      COMPONENTS Interpreter Development.Module
      OPTIONAL_COMPONENTS Development.Embed)
  else()
    find_package(
      Python3 REQUIRED
      COMPONENTS Interpreter Development.Module
      OPTIONAL_COMPONENTS Development.Embed)
  endif()

  if(NOT Python3_FOUND)
    message(FATAL_ERROR "Python not found")
  endif()

  set(PYBIND11_FINDPYTHON ON)

  prepare_package(
    pybind11
    URL
    "https://github.com/pybind/pybind11/archive/v2.13.6.zip"
    URL_HASH
    "SHA256=d0a116e91f64a4a2d8fb7590c34242df92258a61ec644b79127951e821b47be6"
    EXCLUDE_FROM_ALL)
  list(APPEND qbdi_deps pybind11)
endif()

# Fetch all deps
# ==============
FetchContent_MakeAvailable(${qbdi_deps})

# Fetch LLVM
# ==========
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/llvm")
include(QBDI_llvm)

# Spdlog Configure
# ================
set_property(TARGET spdlog PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(
  spdlog INTERFACE SPDLOG_NO_TLS=1 SPDLOG_NO_THREAD_ID=1 SPDLOG_NO_EXCEPTIONS=1)

if(QBDI_LOG_DEBUG)
  target_compile_definitions(spdlog
                             INTERFACE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
else()
  target_compile_definitions(spdlog
                             INTERFACE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO)
endif()

# remove Threads::Threads for android
if(ANDROID)
  get_target_property(SPDLOG_LIB spdlog INTERFACE_LINK_LIBRARIES)
  list(REMOVE_ITEM SPDLOG_LIB Threads::Threads)
  set_property(TARGET spdlog PROPERTY INTERFACE_LINK_LIBRARIES "${SPDLOG_LIB}")
endif()

if(QBDI_PLATFORM_WINDOWS)

else()
  set(SPDLOG_QBDI_CXX_FLAGS
      -ffunction-sections -fdata-sections -fvisibility-inlines-hidden
      -fvisibility=hidden -fPIC -fno-rtti)
  target_compile_options(
    spdlog PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${SPDLOG_QBDI_CXX_FLAGS}>)
endif()
