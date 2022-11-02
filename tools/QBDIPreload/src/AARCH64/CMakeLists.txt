if(QBDI_PLATFORM_LINUX)
  target_sources(QBDIPreload
                 PRIVATE "${CMAKE_CURRENT_LIST_DIR}/linux_AARCH64.c")
elseif(QBDI_PLATFORM_OSX)
  target_sources(QBDIPreload PRIVATE "${CMAKE_CURRENT_LIST_DIR}/osx_AARCH64.c")
else()
  message(FATAL_ERROR "No QBDIPreload for architecture ${QBDI_ARCH}")
endif()
