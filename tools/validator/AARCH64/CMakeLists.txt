target_sources(validator
               PRIVATE "${CMAKE_CURRENT_LIST_DIR}/validatorengine_AARCH64.cpp")

if(QBDI_PLATFORM_LINUX OR QBDI_PLATFORM_ANDROID)
  target_sources(validator
                 PRIVATE "${CMAKE_CURRENT_LIST_DIR}/linux_AARCH64.cpp")
elseif(QBDI_PLATFORM_OSX)
  # no stub
else()
  message(FATAL_ERROR "No stub for platform ${QBDI_PLATFORM}")
endif()
