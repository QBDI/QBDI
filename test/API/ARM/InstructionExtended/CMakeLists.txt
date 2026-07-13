target_sources(
  QBDITest
  PRIVATE "${CMAKE_CURRENT_LIST_DIR}/BranchTest.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/ExclusiveMonitorTest.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/PCDataProcessingTest.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/ThumbBranchTest.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/ThumbExclusiveMonitorTest.cpp")
