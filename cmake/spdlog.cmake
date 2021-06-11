if(__add_qbdi_spdlog)
  return()
endif()
set(__add_qbdi_spdlog ON)

include(FetchContent)

# spdlog
# ======
set(SPDLOG_VERSION 1.8.5)

FetchContent_Declare(
  spdlog-project
  URL "https://github.com/gabime/spdlog/archive/refs/tags/v${SPDLOG_VERSION}.zip"
  URL_HASH
    SHA256=6e66c8ed4c014b0fb00c74d34eea95b5d34f6e4b51b746b1ea863dc3c2e854fd
  DOWNLOAD_DIR "${CMAKE_SOURCE_DIR}/third-party/spdlog-project-download")

FetchContent_GetProperties(spdlog-project)
if(NOT spdlog-project_POPULATED)
  # Fetch the content using previously declared details
  FetchContent_Populate(spdlog-project)

  add_subdirectory(${spdlog-project_SOURCE_DIR} ${spdlog-project_BINARY_DIR}
                   EXCLUDE_FROM_ALL)
endif()

target_compile_definitions(
  spdlog_header_only INTERFACE SPDLOG_NO_TLS=1 SPDLOG_NO_THREAD_ID=1
                               SPDLOG_NO_DATETIME=1)

if(QBDI_LOG_DEBUG)
  target_compile_definitions(
    spdlog_header_only INTERFACE SPDLOG_DEBUG_ON=1
                                 SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
else()
  target_compile_definitions(spdlog_header_only
                             INTERFACE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO)
endif()

# remove Threads::Threads for android
if(ANDROID)
  get_target_property(SPDLOG_LIB spdlog_header_only INTERFACE_LINK_LIBRARIES)
  list(REMOVE_ITEM SPDLOG_LIB Threads::Threads)
  set_property(TARGET spdlog_header_only PROPERTY INTERFACE_LINK_LIBRARIES
                                                  "${SPDLOG_LIB}")
endif()
