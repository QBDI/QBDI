if(QBDI_PLATFORM_LINUX)
  target_sources(QBDIPreload
                 PRIVATE "${CMAKE_CURRENT_LIST_DIR}/linux_AARCH64.c")
elseif(QBDI_PLATFORM_MACOS)
  target_sources(QBDIPreload
                 PRIVATE "${CMAKE_CURRENT_LIST_DIR}/macos_AARCH64.c")
else()
  message(FATAL_ERROR "No QBDIPreload for architecture ${QBDI_ARCH}")
endif()
