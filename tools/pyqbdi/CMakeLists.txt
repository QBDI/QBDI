set(SOURCES "pyqbdi.cpp")

if(HAS_QBDIPRELOAD AND (NOT (${ARCH} STREQUAL "ARM" )))
    unset(PYTHON_EXECUTABLE CACHE)
    unset(PYTHON_INCLUDE_DIR CACHE)
    # Find Python 2.7
    find_package(PythonInterp 2.7 EXACT REQUIRED)
    if(NOT PYTHON_INCLUDE_DIRS)
        set(PYTHON_INCLUDE_DIRS "$ENV{PYTHON_INCLUDE_DIRS}")
    endif()
    if(NOT PYTHON_LIBRARIES)
        set(PYTHON_LIBRARIES "$ENV{PYTHON_LIBRARIES}")
    endif()
    if(NOT PYTHON_INCLUDE_DIRS AND NOT PYTHON_LIBRARIES)
        find_package(PythonLibs 2.7 EXACT REQUIRED)
    elseif(NOT (PYTHON_INCLUDE_DIRS AND PYTHON_LIBRARIES))
        message(FATAL_ERROR "Inconsistent PYTHON_INCLUDE_DIRS and PYTHON_LIBRARIES")
    endif()
    include_directories(${PYTHON_INCLUDE_DIRS})
    add_definitions("-DPYTHON_LIBRARIES=\"${PYTHON_LIBRARIES}\"")

    add_library(pyqbdi SHARED ${SOURCES})
    # Add build directory as include path for the mach_exc.h header
    include_directories(pyqbdi PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        ${CMAKE_CURRENT_SOURCE_DIR}/../QBDIPreload/include
        )
    target_link_libraries(pyqbdi QBDIPreload QBDI ${PYTHON_LIBRARIES})
    install(TARGETS pyqbdi
    LIBRARY DESTINATION lib)
endif()
