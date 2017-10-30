configure_file(qbdi_cpp.doxygen.in ${CMAKE_SOURCE_DIR}/docs/qbdi_cpp.doxygen)
configure_file(qbdi_c.doxygen.in ${CMAKE_SOURCE_DIR}/docs/qbdi_c.doxygen)
configure_file(tools.doxygen.in ${CMAKE_SOURCE_DIR}/docs/tools.doxygen)

add_custom_target(docs
    DEPENDS docs-doxygen docs-sphinx
)

add_custom_target(docs-doxygen
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs
    COMMAND doxygen qbdi_cpp.doxygen
    COMMAND doxygen qbdi_c.doxygen
    COMMAND doxygen tools.doxygen
)

add_custom_target(docs-sphinx
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs/source
    DEPENDS docs-doxygen
    COMMAND sphinx-build -a -b html . ../build
)

add_custom_target(docs-sphinx-fast
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs/source
    COMMAND sphinx-build -a -b html . ../build
)
