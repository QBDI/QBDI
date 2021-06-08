if(__add_qbdi_spdlog)
  return()
endif()
set(__add_qbdi_spdlog ON)

include(FetchContent)

# spdlog
# ======
set(SPDLOG_VERSION 1.8.3)

FetchContent_Declare(
  spdlog-project
  URL "https://github.com/gabime/spdlog/archive/refs/tags/v${SPDLOG_VERSION}.zip"
  URL_HASH
    SHA256=16368df52019a36564b72bc792829a9ce61a3f318905f9a1ac38d949507dce51)

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
