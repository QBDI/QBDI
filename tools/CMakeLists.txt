if((HAS_QBDIPRELOAD) AND (TOOLS_QBDIPRELOAD))
    message(STATUS "Compile QBDIPreload")
    # Add QBDI preload library
    add_subdirectory(QBDIPreload)

    if(TOOLS_VALIDATOR)
        message(STATUS "Compile validator")
        # Add validator
        add_subdirectory(validator)
    endif()

endif()

if(TOOLS_PYQBDI)
    message(STATUS "Compile PyQBDI")
    # Add pyqbdi
    add_subdirectory(pyqbdi)
endif()
