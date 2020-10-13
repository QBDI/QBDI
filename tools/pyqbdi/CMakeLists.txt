
if(QBDI_ARCH_X86 OR QBDI_ARCH_X86_64)
    include(ExternalProject)

    if(NOT PYTHON_VERSION)
        set(PYTHON_VERSION 3)
    endif()
    message(STATUS "Python version: ${PYTHON_VERSION}")
    find_package(PythonInterp "${PYTHON_VERSION}" EXACT REQUIRED)
    if(NOT PYTHON_LIBRARIES)
        set(PYTHON_LIBRARIES "$ENV{PYTHON_LIBRARIES}")
    endif()
    if(NOT PYTHON_LIBRARIES)
        find_package(PythonLibs "${PYTHON_VERSION}" EXACT)
    endif()

    if(NOT PYTHON_INCLUDE_DIRS)
        execute_process(
            COMMAND "${PYTHON_EXECUTABLE}" -c "from sysconfig import get_paths; print(get_paths()['include'])"
            OUTPUT_VARIABLE PYTHON_INCLUDE_DIRS
            OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif()

    execute_process(
        COMMAND "${PYTHON_EXECUTABLE}" -c "import platform; print(platform.architecture()[0])"
        OUTPUT_VARIABLE PYTHON_BIT
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    message(STATUS "Python lib:     ${PYTHON_LIBRARIES}")
    message(STATUS "Python include: ${PYTHON_INCLUDE_DIRS}")
    message(STATUS "Python interpreter: ${PYTHON_EXECUTABLE}")
    message(STATUS "Python ${PYTHON_BIT}")

    # verify we compile pyqbdi tools with the good version of python
    if ((${PYTHON_BIT} STREQUAL "64bit") AND NOT QBDI_ARCH_X86_64)
        message(FATAL_ERROR "PyQBDI with Python ${PYTHON_BIT} is not compatible with the architecture ${QBDI_ARCH}")
    elseif ((${PYTHON_BIT} STREQUAL "32bit") AND NOT QBDI_ARCH_X86)
        message(FATAL_ERROR "PyQBDI with Python ${PYTHON_BIT} is not compatible with the architecture ${QBDI_ARCH}")
    endif()

    ExternalProject_Add(pyqbdi_pybind11
        URL                 "https://github.com/pybind/pybind11/archive/v2.4.3.zip"
        URL_HASH            SHA256=f1cc1e9c2836f9873aefdaf76a3280a55aae51068c759b27499a9cf34090d364
        CONFIGURE_COMMAND   ""
        BUILD_COMMAND       ""
        INSTALL_COMMAND     "")
    ExternalProject_get_property(pyqbdi_pybind11 SOURCE_DIR)
    set(PYBIND11_SOURCE_DIR "${SOURCE_DIR}")

    add_library(pyqbdi_utils INTERFACE)
    add_dependencies(pyqbdi_utils pyqbdi_pybind11)

    add_library(pyqbdi_module INTERFACE)
    add_dependencies(pyqbdi_module pyqbdi_pybind11)

    if(QBDI_PLATFORM_WINDOWS)
        target_compile_options(pyqbdi_utils INTERFACE "/GR" "/EHsc")
        target_compile_options(pyqbdi_module INTERFACE "/GR" "/EHsc")
    else()
        target_compile_options(pyqbdi_utils INTERFACE "-s" "-frtti")
        target_compile_options(pyqbdi_module INTERFACE "-s" "-frtti")
    endif()

    include("${CMAKE_CURRENT_SOURCE_DIR}/utils/CMakeLists.txt")
    include("${CMAKE_CURRENT_SOURCE_DIR}/binding/CMakeLists.txt")

    ##
    # Shared library with QBDIPreload (if available)
    ##
    set(PYQBDI_SRC_MODULE
      "${CMAKE_CURRENT_SOURCE_DIR}/pyqbdi_module.cpp"
    )
    add_library(pyqbdi SHARED ${PYQBDI_SRC_MODULE})
    target_include_directories(pyqbdi PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        "${PYBIND11_SOURCE_DIR}/include"
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PYTHON_INCLUDE_DIRS}
    )

    if ((HAS_QBDIPRELOAD) AND (TOOLS_QBDIPRELOAD))
        target_sources(pyqbdi PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/preload.cpp")
        target_include_directories(pyqbdi PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../QBDIPreload/include")
        target_link_libraries(pyqbdi PRIVATE QBDIPreload)
    endif()

    target_link_libraries(pyqbdi PRIVATE pyqbdi_module pyqbdi_utils QBDI_static)
    set_target_properties(pyqbdi
        PROPERTIES
            CXX_STANDARD 14
            CXX_STANDARD_REQUIRED ON
    )

    if (PYTHON_LIBRARIES)
        set_target_properties(pyqbdi PROPERTIES COMPILE_DEFINITIONS "PYTHON_LIBRARIES=\"${PYTHON_LIBRARIES}\"")
        target_link_libraries(pyqbdi PRIVATE ${PYTHON_LIBRARIES})
    endif()

    set_target_properties(pyqbdi PROPERTIES PREFIX "")

    if (QBDI_PLATFORM_OSX)
        set_target_properties(pyqbdi PROPERTIES MACOSX_RPATH ".")
        set_target_properties(pyqbdi PROPERTIES LINK_FLAGS "-undefined dynamic_lookup ")
        set_target_properties(pyqbdi PROPERTIES SUFFIX ".so")
    elseif (QBDI_PLATFORM_LINUX)
        set_target_properties(pyqbdi PROPERTIES SUFFIX ".so")
    elseif (QBDI_PLATFORM_WINDOWS)
        set_target_properties(pyqbdi PROPERTIES SUFFIX ".pyd")
    endif()

    if (PYQBDI_OUTPUT_DIRECTORY)
        message(STATUS "export PYQBDI to ${PYQBDI_OUTPUT_DIRECTORY}/")
        add_custom_command(TARGET pyqbdi POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:pyqbdi> ${PYQBDI_OUTPUT_DIRECTORY}/$<TARGET_FILE_NAME:pyqbdi>
        )
        add_custom_command(TARGET pyqbdi POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/pyqbdipreload.py" "${PYQBDI_OUTPUT_DIRECTORY}/pyqbdipreload.py"
        )
    endif()

    ##
    # Library for doc
    ##
    add_custom_command(TARGET pyqbdi POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:pyqbdi> ${PROJECT_BINARY_DIR}/docs/$<TARGET_FILE_NAME:pyqbdi>
    )

endif()

