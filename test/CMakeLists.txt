# test
if(QBDI_TEST)

  add_executable(QBDITest "${CMAKE_CURRENT_LIST_DIR}/QBDITest.cpp")
  add_signature(QBDITest)

  include("${CMAKE_CURRENT_LIST_DIR}/API/CMakeLists.txt")
  include("${CMAKE_CURRENT_LIST_DIR}/ExecBlock/CMakeLists.txt")
  include("${CMAKE_CURRENT_LIST_DIR}/Patch/CMakeLists.txt")
  include("${CMAKE_CURRENT_LIST_DIR}/Miscs/CMakeLists.txt")
  include("${CMAKE_CURRENT_LIST_DIR}/TestSetup/CMakeLists.txt")

  target_include_directories(
    QBDITest
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/../include"
            "${CMAKE_CURRENT_SOURCE_DIR}/../include"
            "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/../src")

  target_link_libraries(QBDITest QBDI_static qbdi-llvm Catch2::Catch2 spdlog)
  target_compile_options(QBDITest
                         PUBLIC $<$<COMPILE_LANGUAGE:C>:${QBDI_COMMON_C_FLAGS}>)
  target_compile_options(
    QBDITest PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${QBDI_COMMON_CXX_FLAGS}>)
  target_compile_definitions(QBDITest PUBLIC ${QBDI_COMMON_DEFINITION})

  set_target_properties(QBDITest PROPERTIES CXX_STANDARD 17
                                            CXX_STANDARD_REQUIRED ON)

  add_test(NAME QBDITest COMMAND QBDITest)
endif()

# Benchmark
if(QBDI_BENCHMARK)
  add_executable(QBDIBenchmark "${CMAKE_CURRENT_LIST_DIR}/QBDIBenchmark.cpp")
  add_signature(QBDIBenchmark)

  include("${CMAKE_CURRENT_LIST_DIR}/Benchmark/CMakeLists.txt")

  target_include_directories(
    QBDIBenchmark
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/../include"
            "${CMAKE_CURRENT_SOURCE_DIR}/../include"
            "${CMAKE_CURRENT_SOURCE_DIR}")

  target_compile_options(QBDIBenchmark
                         PUBLIC $<$<COMPILE_LANGUAGE:C>:${QBDI_COMMON_C_FLAGS}>)
  target_compile_options(
    QBDIBenchmark PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${QBDI_COMMON_CXX_FLAGS}>)
  target_link_libraries(QBDIBenchmark QBDI_static Catch2::Catch2)

  set_target_properties(QBDIBenchmark PROPERTIES CXX_STANDARD 17
                                                 CXX_STANDARD_REQUIRED ON)
endif()
