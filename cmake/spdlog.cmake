if(__add_qbdi_spdlog)
  return()
endif()
set(__add_qbdi_spdlog ON)
include(ExternalProject)

# spdlog
# ======
set(SPDLOG_VERSION 1.8.3)
set(SPDLOG_URL
    "https://github.com/gabime/spdlog/archive/refs/tags/v${SPDLOG_VERSION}.zip")
ExternalProject_Add(
  spdlog-project
  URL ${SPDLOG_URL}
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND "")
ExternalProject_Get_Property(spdlog-project SOURCE_DIR)
set(SPDLOG_SOURCE_DIR "${SOURCE_DIR}")

add_library(spdlog INTERFACE)
add_dependencies(spdlog spdlog-project)
target_include_directories(spdlog INTERFACE "${SPDLOG_SOURCE_DIR}/include/")

if(QBDI_PLATFORM_ANDROID)
  target_link_libraries(spdlog INTERFACE log)
endif()
