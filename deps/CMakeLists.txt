set(DEPENDENCY_SATISFIED On PARENT_SCOPE)

# LLVM

find_package(PythonInterp 3 REQUIRED)

add_custom_target(llvm
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/llvm/build.py prepare ${PLATFORM}
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/llvm/build.py build ${PLATFORM}
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/llvm/build.py package ${PLATFORM}
)

file(GLOB LLVMIncludeFile LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/deps/llvm/${PLATFORM}/include/*")
file(GLOB LLVMLibFile LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/deps/llvm/${PLATFORM}/lib/*")

if (NOT (LLVMLibFile AND LLVMIncludeFile))

    set(DEPENDENCY_SATISFIED Off PARENT_SCOPE)
    message(WARNING "\n"
        "    LLVM is not build for this platform,\n"
        "    first execute make llvm then rerun cmake\n")
endif()

# Google Test
if(TEST_QBDI)
    if(${OS} STREQUAL "win")
    add_custom_target(gtest
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/deps/gtest/${PLATFORM}/
        COMMAND python build.py prepare
        COMMAND python build.py build
        COMMAND python build.py package
    )
    else()
    add_custom_target(gtest
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/deps/gtest/${PLATFORM}/
        COMMAND sh build.sh prepare
        COMMAND sh build.sh build
        COMMAND sh build.sh package
    )
    endif()

    file(GLOB GTestIncludeFile LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/deps/gtest/${PLATFORM}/include/*")
    file(GLOB GTestLibFile LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/deps/gtest/${PLATFORM}/lib/*")

    if (NOT (GTestLibFile AND GTestIncludeFile))

        set(DEPENDENCY_SATISFIED Off PARENT_SCOPE)
        message(WARNING "\n"
            "    Google Test is not build for this platform,\n"
            "    first execute make gtest then rerun cmake\n")
    endif()
endif()
