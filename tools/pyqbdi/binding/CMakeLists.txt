set(BINDING_PYTHON_SRC
  "${CMAKE_CURRENT_LIST_DIR}/Callback.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Errors.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/InstAnalysis.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Logs.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Memory.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Range.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/State_${QBDI_ARCH}.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/VM.cpp"
)


source_group("Source Files\\BINDING" FILES ${BINDING_PYTHON_SRC})

target_sources(pyqbdi_module INTERFACE "${BINDING_PYTHON_SRC}")


