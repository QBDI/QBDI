set(DEPENDENCY_SATISFIED On PARENT_SCOPE)

# LLVM

unset(PYTHON_EXECUTABLE CACHE)
find_package(PythonInterp 3 REQUIRED)

add_custom_target(llvm
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/llvm/build.py prepare ${PLATFORM}
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/llvm/build.py build ${PLATFORM}
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/llvm/build.py package ${PLATFORM}
)

if(NOT (EXISTS "${CMAKE_SOURCE_DIR}/deps/llvm/${PLATFORM}/include/" AND
   EXISTS "${CMAKE_SOURCE_DIR}/deps/llvm/${PLATFORM}/lib/"))

    set(DEPENDENCY_SATISFIED Off PARENT_SCOPE)
    message(WARNING "
    LLVM is not build for this platform, 
    first execute make llvm then rerun cmake
    ")
endif()

# Google Test

if(${PLATFORM} STREQUAL win-X86_64)
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

if(NOT (EXISTS "${CMAKE_SOURCE_DIR}/deps/gtest/${PLATFORM}/include/" AND
   EXISTS "${CMAKE_SOURCE_DIR}/deps/gtest/${PLATFORM}/lib/"))

    set(DEPENDENCY_SATISFIED Off PARENT_SCOPE)
    message(WARNING "
    Google Test is not build for this platform, 
    first execute make gtest then rerun cmake
    ")
endif()
