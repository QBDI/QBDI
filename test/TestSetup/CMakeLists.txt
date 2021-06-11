set(SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/InMemoryAssembler.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ShellcodeTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/LLVMTestEnv.cpp")

target_sources(QBDITest PUBLIC "${SOURCES}")
