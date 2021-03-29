set(BINDING_PYTHON_SRC "${CMAKE_CURRENT_LIST_DIR}/Memory.cpp"
                       "${CMAKE_CURRENT_LIST_DIR}/Float.cpp")

target_sources(pyqbdi_utils INTERFACE "${BINDING_PYTHON_SRC}")
