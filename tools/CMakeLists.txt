if(HAS_QBDIPRELOAD)
    # Add QBDI preload library
    add_subdirectory(QBDIPreload)

    # Add validator
    add_subdirectory(validator)
endif()
