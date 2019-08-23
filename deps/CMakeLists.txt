set(DEPENDENCY_SATISFIED On PARENT_SCOPE)

# LLVM

find_package(PythonInterp 3 REQUIRED)

add_custom_target(llvm
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/llvm/build.py prepare ${QBDI_PLATFORM}-${QBDI_ARCH}
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/llvm/build.py build   ${QBDI_PLATFORM}-${QBDI_ARCH}
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/llvm/build.py package ${QBDI_PLATFORM}-${QBDI_ARCH}
)

file(GLOB LLVMIncludeFile LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/deps/llvm/${QBDI_PLATFORM}-${QBDI_ARCH}/include/*")
file(GLOB LLVMLibFile LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/deps/llvm/${QBDI_PLATFORM}-${QBDI_ARCH}/lib/*")

if (NOT (LLVMLibFile AND LLVMIncludeFile))

    set(DEPENDENCY_SATISFIED Off PARENT_SCOPE)
    message(WARNING "\n"
        "    LLVM is not build for this platform,\n"
        "    first execute make|ninja llvm then rerun cmake\n")
endif()

# Google Test
if(QBDI_TEST)
    if(QBDI_PLATFORM_WINDOWS)
    add_custom_target(gtest
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/deps/gtest/${QBDI_PLATFORM}-${QBDI_ARCH}/
        COMMAND ${PYTHON_EXECUTABLE} build.py prepare
        COMMAND ${PYTHON_EXECUTABLE} build.py build
        COMMAND ${PYTHON_EXECUTABLE} build.py package
    )
    else()
    add_custom_target(gtest
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/deps/gtest/${QBDI_PLATFORM}-${QBDI_ARCH}/
        COMMAND sh build.sh prepare
        COMMAND sh build.sh build
        COMMAND sh build.sh package
    )
    endif()

    file(GLOB GTestIncludeFile LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/deps/gtest/${QBDI_PLATFORM}-${QBDI_ARCH}/include/*")
    file(GLOB GTestLibFile LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/deps/gtest/${QBDI_PLATFORM}-${QBDI_ARCH}/lib/*")

    if (NOT (GTestLibFile AND GTestIncludeFile))

        set(DEPENDENCY_SATISFIED Off PARENT_SCOPE)
        message(WARNING "\n"
            "    Google Test is not build for this platform,\n"
            "    first execute make|ninja gtest then rerun cmake\n")
    endif()
endif()
