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
  docs-install-jsdoc
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND npm install jsdoc)

execute_process(COMMAND npm install jsdoc
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
execute_process(
  COMMAND npm root
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  OUTPUT_VARIABLE NPM_JSDOC_ROOT
  OUTPUT_STRIP_TRAILING_WHITESPACE)

add_custom_target(
  docs-sphinx
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/source
  DEPENDS docs-doxygen pyqbdi
  COMMAND
    ${CMAKE_COMMAND} -E env
    "PYTHONPATH=${PROJECT_BINARY_DIR}/docs:$ENV{PYTHONPATH}"
    "SPHINX_JS_NODE_MODULES=${NPM_JSDOC_ROOT}"
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
