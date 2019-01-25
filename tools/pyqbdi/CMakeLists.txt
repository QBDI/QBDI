set(SOURCES "pyqbdi.cpp")

if(HAS_QBDIPRELOAD AND (NOT ${ARCH} STREQUAL "ARM"))
    # Find Python 2.7 & 3
    if( CMAKE_VERSION VERSION_LESS "3.12.0") 
        find_package(PythonInterp 2.7 REQUIRED)
        find_package(PythonInterp 3 REQUIRED)
    else()
        find_package(Python2 COMPONENTS Interpreter REQUIRED)
        find_package(Python3 COMPONENTS Interpreter REQUIRED)
    endif()

    # var include directory for py2
    if(NOT PYTHON2_INCLUDE_DIRS)
        set(PYTHON2_INCLUDE_DIRS "$ENV{PYTHON2_INCLUDE_DIRS}")
    endif()
    if(NOT PYTHON2_INCLUDE_DIRS)
        set(PYTHON2_INCLUDE_DIRS "${PYTHON_INCLUDE_DIRS}")
    endif()
    if(NOT PYTHON2_INCLUDE_DIRS)
        set(PYTHON2_INCLUDE_DIRS "$ENV{PYTHON_INCLUDE_DIRS}")
    endif()

    # var include directory for py3
    if(NOT PYTHON3_INCLUDE_DIRS)
        set(PYTHON3_INCLUDE_DIRS "$ENV{PYTHON3_INCLUDE_DIRS}")
    endif()
    if(NOT PYTHON3_INCLUDE_DIRS)
        set(PYTHON3_INCLUDE_DIRS "${PYTHON_INCLUDE_DIRS}")
    endif()
    if(NOT PYTHON3_INCLUDE_DIRS)
        set(PYTHON3_INCLUDE_DIRS "$ENV{PYTHON_INCLUDE_DIRS}")
    endif()

    # var libraries for py2
    if(NOT PYTHON2_LIBRARIES)
        set(PYTHON2_LIBRARIES "$ENV{PYTHON2_LIBRARIES}")
    endif()
    if(NOT PYTHON2_LIBRARIES)
        set(PYTHON2_LIBRARIES "${PYTHON_LIBRARIES}")
    endif()
    if(NOT PYTHON2_LIBRARIES)
        set(PYTHON2_LIBRARIES "$ENV{PYTHON_LIBRARIES}")
    endif()

    # var libraries for py3
    if(NOT PYTHON3_LIBRARIES)
        set(PYTHON3_LIBRARIES "$ENV{PYTHON3_LIBRARIES}")
    endif()
    if(NOT PYTHON3_LIBRARIES)
        set(PYTHON3_LIBRARIES "${PYTHON_LIBRARIES}")
    endif()
    if(NOT PYTHON3_LIBRARIES)
        set(PYTHON3_LIBRARIES "$ENV{PYTHON_LIBRARIES}")
    endif()

    # missing include directory and library for py2
    if(NOT PYTHON2_INCLUDE_DIRS AND NOT PYTHON2_LIBRARIES)
        if( CMAKE_VERSION VERSION_LESS "3.12.0") 
            find_package(PythonLibs 2.7 REQUIRED)
            set(PYTHON2_INCLUDE_DIRS "${PYTHON_INCLUDE_DIRS}")
            set(PYTHON2_LIBRARIES "${PYTHON_LIBRARIES}")
        else()
            find_package(Python2 COMPONENTS Development REQUIRED)
            set(PYTHON2_INCLUDE_DIRS "${Python2_INCLUDE_DIRS}")
            set(PYTHON2_LIBRARIES "${Python2_LIBRARIES}")
        endif()
    elseif(NOT (PYTHON2_INCLUDE_DIRS AND PYTHON2_LIBRARIES))
        message(FATAL_ERROR "Inconsistent PYTHON2_INCLUDE_DIRS and PYTHON2_LIBRARIES")
    endif()

    # missing include directory and library for py3
    if(NOT PYTHON3_INCLUDE_DIRS AND NOT PYTHON3_LIBRARIES)
        if( CMAKE_VERSION VERSION_LESS "3.12.0") 
            find_package(PythonLibs 3 REQUIRED)
            set(PYTHON3_INCLUDE_DIRS "${PYTHON_INCLUDE_DIRS}")
            set(PYTHON3_LIBRARIES "${PYTHON_LIBRARIES}")
        else()
            find_package(Python3 COMPONENTS Development REQUIRED)
            set(PYTHON3_INCLUDE_DIRS "${Python3_INCLUDE_DIRS}")
            set(PYTHON3_LIBRARIES "${Python3_LIBRARIES}")
        endif()
    elseif(NOT (PYTHON3_INCLUDE_DIRS AND PYTHON3_LIBRARIES))
        message(FATAL_ERROR "Inconsistent PYTHON3_INCLUDE_DIRS and PYTHON3_LIBRARIES")
    endif()

    # compile pyqbdi for py2

    add_library(pyqbdi SHARED ${SOURCES})
    target_include_directories(pyqbdi PRIVATE ${PYTHON2_INCLUDE_DIRS})
    set_target_properties(pyqbdi PROPERTIES COMPILE_DEFINITIONS "PYTHON_LIBRARIES=\"${PYTHON2_LIBRARIES}\"")
    # Add build directory as include path for the mach_exc.h header
    target_include_directories(pyqbdi PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        ${CMAKE_CURRENT_SOURCE_DIR}/../QBDIPreload/include
        )
    target_link_libraries(pyqbdi QBDIPreload QBDI_lib ${PYTHON2_LIBRARIES})

    # compile pyqbdi for py3

    add_library(pyqbdi3 SHARED ${SOURCES})
    target_include_directories(pyqbdi3 PRIVATE ${PYTHON3_INCLUDE_DIRS})
    set_target_properties(pyqbdi3 PROPERTIES COMPILE_DEFINITIONS "PYTHON_LIBRARIES=\"${PYTHON3_LIBRARIES}\"")
    # Add build directory as include path for the mach_exc.h header
    target_include_directories(pyqbdi3 PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        ${CMAKE_CURRENT_SOURCE_DIR}/../QBDIPreload/include
        )
    target_link_libraries(pyqbdi3 QBDIPreload QBDI_lib ${PYTHON3_LIBRARIES})

    # install lib

    install(TARGETS pyqbdi
    LIBRARY DESTINATION lib)

    install(TARGETS pyqbdi3
    LIBRARY DESTINATION lib)
endif()
