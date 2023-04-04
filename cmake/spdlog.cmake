if(__add_qbdi_spdlog)
  return()
endif()
set(__add_qbdi_spdlog ON)

include(FetchContent)

# spdlog
# ======
set(SPDLOG_VERSION 1.10.0)

FetchContent_Declare(
  spdlog
  URL "https://github.com/gabime/spdlog/archive/refs/tags/v${SPDLOG_VERSION}.zip"
  URL_HASH
    "SHA256=7be28ff05d32a8a11cfba94381e820dd2842835f7f319f843993101bcab44b66"
  DOWNLOAD_DIR "${QBDI_THIRD_PARTY_DIRECTORY}/spdlog-download")

FetchContent_GetProperties(spdlog)
if(NOT spdlog_POPULATED)
  # Fetch the content using previously declared details
  FetchContent_Populate(spdlog)

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

  add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

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
