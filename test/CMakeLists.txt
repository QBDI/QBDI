
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

    MERGE_STATIC_LIBS(QBDI_static_test QBDI_static \${TEST_LLVM_LIBS})

    set(SOURCES
        QBDITest.cpp
        API/AllocTest.cpp
        API/RangeTest.cpp
        API/VMTest.cpp
        ExecBlock/ExecBlockTest.cpp
        ExecBlock/ExecBlockManagerTest.cpp
        TestSetup/LLVMTestEnv.cpp
        Patch/ComparedExecutor_${QBDI_ARCH}.cpp
        Patch/Instr_${QBDI_BASE_ARCH}Test.cpp
        Patch/Patch_${QBDI_BASE_ARCH}Test.cpp
        Miscs/StringTest.cpp
        TestSetup/InMemoryAssembler.cpp
        TestSetup/ShellcodeTester.cpp
    )

    if (QBDI_ARCH_X86_64 OR QBDI_ARCH_X86)
        set(SOURCES ${SOURCES} "API/MemoryAccessTest.cpp")
    endif()

    if(QBDI_PLATFORM_WINDOWS AND QBDI_ARCH_X86_64)
        set(SOURCES ${SOURCES}
            Patch/WIN64_RunRealExec.asm
        )
    elseif (QBDI_PLATFORM_WINDOWS AND QBDI_ARCH_X86)
        set(SOURCES ${SOURCES}
            Patch/WIN86_RunRealExec.asm
        )
        set_source_files_properties("Patch/WIN86_RunRealExec.asm" PROPERTIES COMPILE_FLAGS "/safeseh")
    endif()

    add_executable(QBDITest ${SOURCES})
    add_signature(QBDITest)

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

    set_property(TARGET QBDITest PROPERTY CXX_STANDARD 14)
    set_property(TARGET QBDITest PROPERTY CXX_STANDARD_REQUIRED ON)
endif()


# Benchmark
if(QBDI_BENCHMARK)
    ExternalProject_Add(SHA256_lib
        GIT_REPOSITORY      "https://github.com/aguinet/sha256_literal.git"
        GIT_PROGRESS        "true"
        GIT_TAG             "d7017a7b4bbc30bc93fb8bd4cf54555986d25ef0"
        CONFIGURE_COMMAND   ""
        BUILD_COMMAND       ""
        INSTALL_COMMAND     "")

    ExternalProject_get_property(SHA256_lib SOURCE_DIR)
    set(SHA256_lib_SOURCE_DIR "${SOURCE_DIR}")

    add_custom_command(OUTPUT "${SHA256_lib_SOURCE_DIR}/sha256_impl.cpp" DEPENDS SHA256_lib
        COMMAND ${CMAKE_COMMAND} -E create_symlink "${SHA256_lib_SOURCE_DIR}/sha256.cpp" "${SHA256_lib_SOURCE_DIR}/sha256_impl.cpp")

    set(SOURCES_BENCHMARK
        QBDIBenchmark.cpp
        Benchmark/Fibonacci.cpp
        Benchmark/SHA256.cpp
        "${SHA256_lib_SOURCE_DIR}/sha256_impl.cpp"
    )

    add_executable(QBDIBenchmark ${SOURCES_BENCHMARK})
    add_signature(QBDIBenchmark)

    target_include_directories(QBDIBenchmark PRIVATE
        "${CMAKE_BINARY_DIR}/include"
        "${CMAKE_BINARY_DIR}/include/QBDI"
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/include/QBDI"
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${Catch2_SOURCE_DIR}/single_include"
        "${SHA256_lib_SOURCE_DIR}"
    )

    add_dependencies(QBDIBenchmark Catch2 SHA256_lib)
    target_link_libraries(QBDIBenchmark
        QBDI_static
        ${LLVM_LIBS}
    )

    set_property(TARGET QBDIBenchmark PROPERTY CXX_STANDARD 14)
    set_property(TARGET QBDIBenchmark PROPERTY CXX_STANDARD_REQUIRED ON)
endif()
