if(QBDI_TOOLS_QBDIPRELOAD)
  # Add QBDI preload library
  add_subdirectory(QBDIPreload)

  if(QBDI_TOOLS_VALIDATOR)
    # Add validator
    add_subdirectory(validator)
  endif()

  if(QBDI_PLATFORM_WINDOWS)
    add_subdirectory(QBDIWinPreloader)
  endif()

endif()

if(QBDI_TOOLS_PYQBDI)
  message(STATUS "Compile PyQBDI")
  # Add pyqbdi
  add_subdirectory(pyqbdi)
endif()
