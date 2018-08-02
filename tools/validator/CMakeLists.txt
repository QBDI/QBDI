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

# Also add build directory as include path for the mach_exc.h header
target_include_directories(validator PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include/QBDI>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../QBDIPreload/include>
    $<INSTALL_INTERFACE:include>
    )
target_link_libraries(validator QBDIPreload QBDI_static)
