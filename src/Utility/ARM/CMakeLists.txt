target_sources(QBDI_src
               INTERFACE "${CMAKE_CURRENT_LIST_DIR}/InstAnalysis_ARM.cpp")

if(QBDI_PLATFORM_IOS)
  target_sources(
    QBDI_src
    INTERFACE "${CMAKE_CURRENT_LIST_DIR}/System_iOS.cpp"
              "${CMAKE_CURRENT_LIST_DIR}/System_darwin.cpp"
              "${CMAKE_CURRENT_LIST_DIR}/server-iOS-jit-user.c")
endif()
