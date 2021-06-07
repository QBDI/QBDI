include(ExternalProject)

ExternalProject_Add(
  Catch2
  GIT_REPOSITORY "https://github.com/catchorg/Catch2.git"
  GIT_PROGRESS "true"
  GIT_TAG "v2.9.2"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND "")

ExternalProject_Get_Property(Catch2 SOURCE_DIR)
set(Catch2_SOURCE_DIR "${SOURCE_DIR}")

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
    PRIVATE "${CMAKE_BINARY_DIR}/include"
            "${CMAKE_BINARY_DIR}/include/QBDI"
            "${CMAKE_SOURCE_DIR}/include"
            "${CMAKE_SOURCE_DIR}/include/QBDI"
            "${CMAKE_CURRENT_SOURCE_DIR}"
            "${CMAKE_CURRENT_SOURCE_DIR}/../src"
            "${Catch2_SOURCE_DIR}/single_include")

  add_dependencies(QBDITest Catch2)
  target_link_libraries(QBDITest QBDI_static qbdi-llvm)
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
            "${CMAKE_CURRENT_SOURCE_DIR}" "${Catch2_SOURCE_DIR}/single_include")

  add_dependencies(QBDIBenchmark Catch2)
  target_compile_options(QBDIBenchmark
                         PUBLIC $<$<COMPILE_LANGUAGE:C>:${QBDI_COMMON_C_FLAGS}>)
  target_compile_options(
    QBDIBenchmark PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${QBDI_COMMON_CXX_FLAGS}>)
  target_link_libraries(QBDIBenchmark QBDI_static)

  set_target_properties(QBDIBenchmark PROPERTIES CXX_STANDARD 17
                                                 CXX_STANDARD_REQUIRED ON)
endif()
