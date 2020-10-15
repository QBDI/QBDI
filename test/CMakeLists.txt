
include(ExternalProject)

ExternalProject_Add(Catch2
    GIT_REPOSITORY      "https://github.com/catchorg/Catch2.git"
    GIT_PROGRESS        "true"
    GIT_TAG             "v2.9.2"
    CONFIGURE_COMMAND   ""
    BUILD_COMMAND       ""
    INSTALL_COMMAND     "")

ExternalProject_get_property(Catch2 SOURCE_DIR)
set(Catch2_SOURCE_DIR "${SOURCE_DIR}")

# test
if (QBDI_TEST)

    # Generate a new static qbdi archive with additionnal llvm
    # needed for the test. The new library allows to remove the llvm library only
    # used by QBDITest for libQBDI.a and avoid error when link QBDItest
    set(_TEST_LLVM_LIBS
        LLVMObject
        LLVMTextAPI
        LLVMCore
        LLVMBitReader
        LLVMBitstreamReader
        LLVMRemarks

      )

    if(QBDI_PLATFORM_WINDOWS)
      PREPEND(TEST_LLVM_LIBS ${CMAKE_SOURCE_DIR}/${LLVM_LIBS_DIR}/ ".lib" ${_TEST_LLVM_LIBS})
    else()
      PREPEND(TEST_LLVM_LIBS ${CMAKE_SOURCE_DIR}/${LLVM_LIBS_DIR}/lib ".a" ${_TEST_LLVM_LIBS})
    endif()
    set(qbditest_static_libs ${TEST_LLVM_LIBS} QBDI_static)

    MERGE_STATIC_LIBS(QBDI_static_test QBDITest_static \${qbditest_static_libs})

    add_executable(QBDITest "${CMAKE_CURRENT_LIST_DIR}/QBDITest.cpp")
    add_signature(QBDITest)

    include("${CMAKE_CURRENT_LIST_DIR}/API/CMakeLists.txt")
    include("${CMAKE_CURRENT_LIST_DIR}/ExecBlock/CMakeLists.txt")
    include("${CMAKE_CURRENT_LIST_DIR}/Patch/CMakeLists.txt")
    include("${CMAKE_CURRENT_LIST_DIR}/Miscs/CMakeLists.txt")
    include("${CMAKE_CURRENT_LIST_DIR}/TestSetup/CMakeLists.txt")

    target_include_directories(QBDITest PRIVATE
        "${CMAKE_BINARY_DIR}/include"
        "${CMAKE_BINARY_DIR}/include/QBDI"
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/include/QBDI"
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../src"
        "${CMAKE_CURRENT_SOURCE_DIR}/../deps/llvm/${QBDI_PLATFORM}-${QBDI_ARCH}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../deps/llvm/${QBDI_PLATFORM}-${QBDI_ARCH}/lib/Target/${QBDI_LLVM_ARCH}"
        "${Catch2_SOURCE_DIR}/single_include"
    )

    add_dependencies(QBDITest Catch2)
    target_link_libraries(QBDITest
        QBDI_static_test
        ${LLVM_LIBS}
    )

    set_target_properties(QBDITest
        PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
    )
endif()

# Benchmark
if(QBDI_BENCHMARK)
    add_executable(QBDIBenchmark "${CMAKE_CURRENT_LIST_DIR}/QBDIBenchmark.cpp")
    add_signature(QBDIBenchmark)

    include("${CMAKE_CURRENT_LIST_DIR}/Benchmark/CMakeLists.txt")

    target_include_directories(QBDIBenchmark PRIVATE
        "${CMAKE_BINARY_DIR}/include"
        "${CMAKE_BINARY_DIR}/include/QBDI"
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/include/QBDI"
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${Catch2_SOURCE_DIR}/single_include"
    )

    add_dependencies(QBDIBenchmark Catch2)
    target_link_libraries(QBDIBenchmark
        QBDI_static
        ${LLVM_LIBS}
    )

    set_target_properties(QBDIBenchmark
        PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
    )
endif()
