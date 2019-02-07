
if(NOT (${ARCH} STREQUAL "ARM" ))
    include(ExternalProject)

    if(NOT PYTHON_VERSION)
        set(PYTHON_VERSION 3)
    endif()
    message(STATUS "Python version: ${PYTHON_VERSION}")
    find_package(PythonInterp "${PYTHON_VERSION}" EXACT REQUIRED)
    if(NOT PYTHON_INCLUDE_DIRS)
        set(PYTHON_INCLUDE_DIRS "$ENV{PYTHON_INCLUDE_DIRS}")
    endif()
    if(NOT PYTHON_LIBRARIES)
        set(PYTHON_LIBRARIES "$ENV{PYTHON_LIBRARIES}")
    endif()
    if(NOT PYTHON_INCLUDE_DIRS AND NOT PYTHON_LIBRARIES)
        find_package(PythonLibs "${PYTHON_VERSION}" EXACT REQUIRED)
    elseif(NOT (PYTHON_INCLUDE_DIRS AND PYTHON_LIBRARIES))
        message(FATAL_ERROR "Inconsistent PYTHON_INCLUDE_DIRS and PYTHON_LIBRARIES")
    endif()

    message(STATUS "Python lib:     ${PYTHON_LIBRARY}")
    message(STATUS "Python include: ${PYTHON_INCLUDE_DIR}")
    message(STATUS "Python interpreter: ${PYTHON_EXECUTABLE}")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s -frtti -DQBDI_PLATFORM_STR='\"${QBDI_PLATFORM}\"' -DQBDI_ARCH_STR='\"${QBDI_ARCH}\"'")

    ExternalProject_Add(pyqbdi_pybind11
        GIT_REPOSITORY      "https://github.com/pybind/pybind11.git"
        GIT_PROGRESS        "true"
        GIT_TAG             "25abf7efba0b2990f5a6dfb0a31bc65c0f2f4d17"
        CONFIGURE_COMMAND   ""
        BUILD_COMMAND       ""
        INSTALL_COMMAND     "")
    ExternalProject_get_property(pyqbdi_pybind11 SOURCE_DIR)
    set(PYBIND11_SOURCE_DIR "${SOURCE_DIR}")

    execute_process(
        COMMAND "${PYTHON_EXECUTABLE}" -c "from distutils import sysconfig as sc; print(sc.get_python_lib(prefix='', plat_specific=True))"
        OUTPUT_VARIABLE PYTHON_SITE
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    add_library(pyqbdi_utils INTERFACE)
    add_dependencies(pyqbdi_utils pyqbdi_pybind11)

    add_library(pyqbdi_module INTERFACE)
    add_dependencies(pyqbdi_module pyqbdi_pybind11)

    include("${CMAKE_CURRENT_SOURCE_DIR}/utils/CMakeLists.txt")
    include("${CMAKE_CURRENT_SOURCE_DIR}/binding/CMakeLists.txt")

    ##
    # Shared library with QBDIPreload (if available)
    ##
    set(PYQBDI_SRC_MODULE
      "${CMAKE_CURRENT_SOURCE_DIR}/pyqbdi_module.cpp"
    )
    add_library(pyqbdi SHARED ${PYQBDI_SRC_MODULE})
    set_target_properties(pyqbdi PROPERTIES COMPILE_DEFINITIONS "PYTHON_LIBRARIES=\"${PYTHON_LIBRARIES}\"")
    target_include_directories(pyqbdi PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        "${PYBIND11_SOURCE_DIR}/include"
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PYTHON_INCLUDE_DIRS}
    )

    if (HAS_QBDIPRELOAD)
        target_sources(pyqbdi PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/preload.cpp")
        target_include_directories(pyqbdi PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../QBDIPreload/include")
        target_link_libraries(pyqbdi PRIVATE QBDIPreload)
    endif()

    target_link_libraries(pyqbdi PRIVATE pyqbdi_module pyqbdi_utils QBDI_static ${PYTHON_LIBRARIES})
    set_target_properties(pyqbdi PROPERTIES PREFIX "")
    install(TARGETS pyqbdi LIBRARY DESTINATION "${PYTHON_SITE}")

    if(${OS} STREQUAL "macOS")
        set_target_properties(pyqbdi PROPERTIES MACOSX_RPATH ".")
        set_target_properties(pyqbdi PROPERTIES LINK_FLAGS "-undefined dynamic_lookup ")
        set_target_properties(pyqbdi PROPERTIES SUFFIX ".so")
    elseif (${OS} STREQUAL "linux")
        set_target_properties(pyqbdi PROPERTIES SUFFIX ".so")
    elseif(${OS} STREQUAL "win")
        set_target_properties(pyqbdi PROPERTIES SUFFIX ".pyd")
    endif()

    ##
    # Library for doc
    ##
    add_library(pyqbdi_docs SHARED EXCLUDE_FROM_ALL "${CMAKE_CURRENT_SOURCE_DIR}/pyqbdi_docs.cpp")
    set_target_properties(pyqbdi_docs PROPERTIES COMPILE_DEFINITIONS "PYTHON_LIBRARIES=\"${PYTHON_LIBRARIES}\"")
    target_include_directories(pyqbdi_docs PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        "${PYBIND11_SOURCE_DIR}/include"
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PYTHON_INCLUDE_DIRS}
    )
    target_link_libraries(pyqbdi_docs PRIVATE pyqbdi_utils ${PYTHON_LIBRARIES})

    add_custom_command(TARGET pyqbdi_docs POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:pyqbdi_docs> ${PROJECT_BINARY_DIR}/docs/pyqbdi.so
    )

endif()

