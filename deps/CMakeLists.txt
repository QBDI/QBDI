set(DEPENDENCY_SATISFIED On PARENT_SCOPE)

# LLVM

if(${OS} STREQUAL "win")
    set(SH "C:\\cygwin64\\bin\\sh")
else()
    set(SH "sh")
endif()

add_custom_target(llvm
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/deps/llvm/${PLATFORM}/
    COMMAND ${SH} build.sh prepare
    COMMAND ${SH} build.sh build
    COMMAND ${SH} build.sh package
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

add_custom_target(gtest
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/deps/gtest/${PLATFORM}/
    COMMAND ${SH} build.sh prepare
    COMMAND ${SH} build.sh build
    COMMAND ${SH} build.sh package
)

if(NOT (EXISTS "${CMAKE_SOURCE_DIR}/deps/gtest/${PLATFORM}/include/" AND
   EXISTS "${CMAKE_SOURCE_DIR}/deps/gtest/${PLATFORM}/lib/"))

    set(DEPENDENCY_SATISFIED Off PARENT_SCOPE)
    message(WARNING "
    Google Test is not build for this platform, 
    first execute make gtest then rerun cmake
    ")
endif()
