target_sources(
  QBDITest
  PRIVATE "${CMAKE_CURRENT_LIST_DIR}/InstAnalysisTest_ARM.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/InstAnalysisTest_Thumb.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/MemoryAccessTest_ARM.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/MemoryAccessTest_ARM_LDM_STM.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/MemoryAccessTest_Thumb.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/MemoryAccessTest_Thumb_LDM_STM.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/VMTest_ARM.cpp")
