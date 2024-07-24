if(__add_qbdi_dependencies)
  return()
endif()
set(__add_qbdi_dependencies ON)

include(FetchContent_local)

# llvm
# ====
include(QBDI_llvm)

set(qbdi_optionnal_deps)

# Catch2
# ======
if(QBDI_BENCHMARK OR QBDI_TEST)
  FetchContent_Declare(
    Catch2
    URL "https://github.com/catchorg/Catch2/archive/refs/tags/v2.13.7.zip"
    URL_HASH
      "SHA256=3f3ccd90ad3a8fbb1beeb15e6db440ccdcbebe378dfd125d07a1f9a587a927e9"
    DOWNLOAD_DIR "${QBDI_THIRD_PARTY_DIRECTORY}/catch2-download"
                 ${FETCHCONTENT_EXCLUDE_FROM_ALL})
  list(APPEND qbdi_optionnal_deps Catch2)
endif()

# pybind11
# ========
if(QBDI_TOOLS_PYQBDI)
  find_package(Python3 COMPONENTS Interpreter Development)
  set(PYTHON_EXECUTABLE "${Python3_EXECUTABLE}")

  FetchContent_Declare(
    pybind11
    URL "https://github.com/pybind/pybind11/archive/v2.11.1.zip"
    URL_HASH
      "SHA256=b011a730c8845bfc265f0f81ee4e5e9e1d354df390836d2a25880e123d021f89"
    DOWNLOAD_DIR "${QBDI_THIRD_PARTY_DIRECTORY}/pybind11-download"
                 ${FETCHCONTENT_EXCLUDE_FROM_ALL})
  list(APPEND qbdi_optionnal_deps pybind11)
endif()

# sha256
# ======
if(QBDI_BENCHMARK)
  FetchContent_Declare(
    sha256_lib
    URL "https://github.com/aguinet/sha256_literal/archive/d7017a7b4bbc30bc93fb8bd4cf54555986d25ef0.zip"
    URL_HASH
      "SHA256=7f7c3771764829ec74ee3f9cfc9df6250051f92d6e7f2e1e11f06a0b48938b2c"
    DOWNLOAD_DIR "${QBDI_THIRD_PARTY_DIRECTORY}/sha256_lib-download"
                 ${FETCHCONTENT_EXCLUDE_FROM_ALL})
  list(APPEND qbdi_optionnal_deps sha256_lib)
endif()

# spdlog
# ======
set(SPDLOG_VERSION 1.10.0)

FetchContent_Declare(
  spdlog
  URL "https://github.com/gabime/spdlog/archive/refs/tags/v${SPDLOG_VERSION}.zip"
  URL_HASH
    "SHA256=7be28ff05d32a8a11cfba94381e820dd2842835f7f319f843993101bcab44b66"
  DOWNLOAD_DIR "${QBDI_THIRD_PARTY_DIRECTORY}/spdlog-download"
               ${FETCHCONTENT_EXCLUDE_FROM_ALL})

set(SPDLOG_NO_EXCEPTIONS
    ON
    CACHE INTERNAL "Enable SPDLOG_NO_EXCEPTIONS")
set(SPDLOG_NO_TLS
    ON
    CACHE INTERNAL "Enable SPDLOG_NO_TLS")
set(SPDLOG_NO_THREAD_ID
    ON
    CACHE INTERNAL "Enable SPDLOG_NO_THREAD_ID")
if(QBDI_LOG_DEBUG)
  set(SPDLOG_ACTIVE_LEVEL
      SPDLOG_LEVEL_DEBUG
      CACHE INTERNAL "Enable SPDLOG_LEVEL_DEBUG level")
endif()

# Download all dependencies
# =========================
fetchcontent_makeavailable_exclude_from_all(spdlog ${qbdi_optionnal_deps})

# configure spdlog
# ================
target_compile_definitions(spdlog INTERFACE SPDLOG_NO_TLS=1
                                            SPDLOG_NO_THREAD_ID=1)

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
