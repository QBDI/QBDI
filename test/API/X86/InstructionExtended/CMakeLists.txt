target_sources(
  QBDITest
  PRIVATE "${CMAKE_CURRENT_LIST_DIR}/PrefixTest.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/PushaPopaTest.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/MemoryJumpCallTest.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/DirectJumpTest.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/RegisterJumpCallTest.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/ConditionalJumpTest.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/PcrelCallTest.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/ReturnTest.cpp")
