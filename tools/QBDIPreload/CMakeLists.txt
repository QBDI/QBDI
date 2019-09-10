set(SOURCES "")

if((${OS} STREQUAL "linux") OR (${OS} STREQUAL "android"))
    set(SOURCES ${SOURCES} "src/linux_preload.c")
elseif(${OS} STREQUAL "macOS")
    # Generate MIG interface at config time in the build directory
    execute_process(WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMAND mig /usr/include/mach/mach_exc.defs)
    set(SOURCES ${SOURCES} "src/darwin_preload.c" "src/darwin_exceptd.c" "mach_excServer.c")
endif()

add_library(QBDIPreload STATIC ${SOURCES})

# Also add build directory as include path for the mach_exc.h header
target_include_directories(QBDIPreload PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
    $<INSTALL_INTERFACE:include>
)

install(TARGETS QBDIPreload
        EXPORT QBDIPreload_targets
        ARCHIVE DESTINATION lib)

install(FILES include/QBDIPreload.h DESTINATION include/)

install(EXPORT QBDIPreload_targets
        FILE QBDIPreloadConfig.cmake
        NAMESPACE QBDIPreload::
        DESTINATION ${PRELOAD_RESOURCES_PREFIX}/cmake)
