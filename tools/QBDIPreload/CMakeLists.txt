set(SOURCES "")

if((${OS} STREQUAL "linux") OR (${OS} STREQUAL "android"))
    set(SOURCES ${SOURCES} "src/linux_preload.c")
elseif(WIN32)
    set(SOURCES ${SOURCES} "src/win_preload.c")
elseif(${OS} STREQUAL "macOS")
    # Generate MIG interface at config time in the build directory
    execute_process(WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMAND mig /usr/include/mach/mach_exc.defs)
    set(SOURCES ${SOURCES} "src/darwin_preload.c" "src/darwin_exceptd.c" "mach_excServer.c")
    # Add build directory as include path for the mach_exc.h header
    include_directories(QBDIPreload PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
endif()

include_directories(QBDIPreload ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(QBDIPreload STATIC ${SOURCES})

install(TARGETS QBDIPreload
        ARCHIVE DESTINATION lib)

install(FILES include/QBDIPreload.h DESTINATION include/)
