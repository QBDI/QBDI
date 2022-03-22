configure_file(qbdi_cpp.doxygen.in ${CMAKE_CURRENT_BINARY_DIR}/qbdi_cpp.doxygen
               @ONLY)
configure_file(qbdi_c.doxygen.in ${CMAKE_CURRENT_BINARY_DIR}/qbdi_c.doxygen
               @ONLY)
configure_file(qbdipreload.doxygen.in
               ${CMAKE_CURRENT_BINARY_DIR}/qbdipreload.doxygen @ONLY)

add_custom_target(docs DEPENDS docs-doxygen docs-sphinx)

add_custom_target(
  docs-doxygen
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND doxygen qbdi_cpp.doxygen
  COMMAND doxygen qbdi_c.doxygen
  COMMAND doxygen qbdipreload.doxygen)

add_custom_target(
  docs-sphinx
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/source
  DEPENDS docs-doxygen pyqbdi
  COMMAND
    ${CMAKE_COMMAND} -E env
    "PYTHONPATH=${PROJECT_BINARY_DIR}/docs:$ENV{PYTHONPATH}"
    "QBDI_DOXYGEN_DIRS=${CMAKE_CURRENT_BINARY_DIR}" sphinx-build -a -b html .
    "${CMAKE_CURRENT_BINARY_DIR}/build")

add_custom_target(
  docs-sphinx-fast
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/source
  DEPENDS pyqbdi
  COMMAND
    ${CMAKE_COMMAND} -E env
    "PYTHONPATH=${PROJECT_BINARY_DIR}/docs:$ENV{PYTHONPATH}"
    "QBDI_DOXYGEN_DIRS=${CMAKE_CURRENT_BINARY_DIR}" sphinx-build -a -b html .
    "${CMAKE_CURRENT_BINARY_DIR}/build")
