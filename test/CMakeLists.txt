include(FetchContent)

if((QBDI_TEST) OR (QBDI_BENCHMARK))
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY "https://github.com/catchorg/Catch2.git"
    GIT_PROGRESS "true"
    GIT_TAG "2c869e17e4803d30b3d5ca5b0d76387b9db97fa5" # v2.9.2
  )

  FetchContent_GetProperties(Catch2)
  if(NOT catch2_POPULATED)
    FetchContent_Populate(Catch2)
    add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR} EXCLUDE_FROM_ALL)
  endif()
endif()

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
    PRIVATE "${CMAKE_BINARY_DIR}/include" "${CMAKE_BINARY_DIR}/include/QBDI"
            "${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/include/QBDI"
            "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/../src")

  target_link_libraries(QBDITest QBDI_static qbdi-llvm Catch2::Catch2)
  target_compile_options(QBDITest
                         PUBLIC $<$<COMPILE_LANGUAGE:C>:${QBDI_COMMON_C_FLAGS}>)
  target_compile_options(
    QBDITest PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${QBDI_COMMON_CXX_FLAGS}>)
  target_compile_definitions(QBDITest PUBLIC ${QBDI_COMMON_DEFINITION})

  set_target_properties(QBDITest PROPERTIES CXX_STANDARD 17
                                            CXX_STANDARD_REQUIRED ON)
endif()

# Benchmark
if(QBDI_BENCHMARK)
  add_executable(QBDIBenchmark "${CMAKE_CURRENT_LIST_DIR}/QBDIBenchmark.cpp")
  add_signature(QBDIBenchmark)

  include("${CMAKE_CURRENT_LIST_DIR}/Benchmark/CMakeLists.txt")

  target_include_directories(
    QBDIBenchmark
    PRIVATE "${CMAKE_BINARY_DIR}/include" "${CMAKE_BINARY_DIR}/include/QBDI"
            "${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/include/QBDI"
            "${CMAKE_CURRENT_SOURCE_DIR}")

  target_compile_options(QBDIBenchmark
                         PUBLIC $<$<COMPILE_LANGUAGE:C>:${QBDI_COMMON_C_FLAGS}>)
  target_compile_options(
    QBDIBenchmark PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${QBDI_COMMON_CXX_FLAGS}>)
  target_link_libraries(QBDIBenchmark QBDI_static Catch2::Catch2)

  set_target_properties(QBDIBenchmark PROPERTIES CXX_STANDARD 17
                                                 CXX_STANDARD_REQUIRED ON)
endif()
