target_sources(
  QBDITest
  PRIVATE "${CMAKE_CURRENT_LIST_DIR}/InstAnalysisTest_AARCH64.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/MemoryAccessTest_AARCH64.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/VMTest_AARCH64.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/VMTest_AARCH64_LocalMonitor.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/VMTest_AARCH64_X28.cpp")

if(NOT QBDI_PLATFORM_OSX)
  target_sources(QBDITest
                 PRIVATE "${CMAKE_CURRENT_LIST_DIR}/VMTest_AARCH64_LDRL.cpp")
endif()
