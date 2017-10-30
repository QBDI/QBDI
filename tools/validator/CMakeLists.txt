set(SOURCES 
    "instrumented.cpp" 
    "master.cpp" 
    "validatorengine.cpp" 
    "pipes.cpp"
)

if((${OS} STREQUAL "linux") OR (${OS} STREQUAL "android"))
    set(SOURCES ${SOURCES} "linux_validator.cpp" "linux_process.cpp")
elseif(${OS} STREQUAL "macOS")
    set(SOURCES ${SOURCES} "darwin_validator.cpp" "darwin_process.cpp")
endif()

add_library(validator SHARED ${SOURCES})
# Add build directory as include path for the mach_exc.h header
include_directories(validator PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    ${CMAKE_CURRENT_SOURCE_DIR}/../QBDIPreload/include
    )
target_link_libraries(validator QBDIPreload QBDI)
