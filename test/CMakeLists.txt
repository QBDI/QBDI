include_directories(
    "."
)

set(SOURCES
    QBDITest.cpp
    API/AllocTest.cpp
    API/MemoryAccessTest.cpp
    API/RangeTest.cpp
    API/VMTest.cpp
    ExecBlock/ExecBlockTest.cpp
    ExecBlock/ExecBlockManagerTest.cpp
    TestSetup/LLVMTestEnv.cpp
    Patch/ComparedExecutor_${ARCH}.cpp
    Patch/Instr_${ARCH}Test.cpp
    Patch/Patch_${ARCH}Test.cpp
    Miscs/StringTest.cpp
    TestSetup/InMemoryAssembler.cpp
    TestSetup/ShellcodeTester.cpp
)

if(${PLATFORM} STREQUAL "win-X86_64")

set(SOURCES ${SOURCES}
    Patch/WIN64_RunRealExec.asm
)

endif()

add_executable(QBDITest ${SOURCES})
add_signature(QBDITest)

if(${PLATFORM} STREQUAL "win-X86_64")

target_link_libraries(QBDITest
    QBDI_static
    gtest.lib
)

else()

target_link_libraries(QBDITest
    QBDI_static
    libgtest.a
)

endif()

set_property(TARGET QBDITest PROPERTY CXX_STANDARD 11)
set_property(TARGET QBDITest PROPERTY CXX_STANDARD_REQUIRED ON)
