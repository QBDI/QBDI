CMake Integration
=================

One architecture
----------------

If you used only one architecture of QBDI in your CMake, you can use the package ``QBDI`` and ``QBDIPreload``:

.. code-block:: cmake

    find_package(QBDI REQUIRED)
    find_package(QBDIPreload REQUIRED) # if available for the platform
    # or
    find_package(QBDI REQUIRED HINTS "${EXTRACT_DIRECTORY}" NO_DEFAULT_PATH)
    find_package(QBDIPreload REQUIRED HINTS "${EXTRACT_DIRECTORY}" NO_DEFAULT_PATH)

Once the package found, you can link your executable with the dynamic or the static library:

.. code-block:: cmake

    add_executable(example example.c)

    target_link_libraries(example QBDI::QBDI)
    # or
    target_link_libraries(example QBDI::QBDI_static)

    add_executable(example_preload example_preload.c)
    target_link_libraries(example_preload QBDI::QBDI_static QBDIPreload::QBDIPreload)


Two architectures
-----------------

If two or more architecture of QBDI are used in the same project, you should use the package ``QBDI<arch>`` and ``QBDIPreload<arch>``.

.. code-block:: cmake

    find_package(QBDIX86 REQUIRED HINTS "${EXTRACT_DIRECTORY_X86}" NO_DEFAULT_PATH)
    find_package(QBDIPreloadX86 REQUIRED HINTS "${EXTRACT_DIRECTORY_X86}" NO_DEFAULT_PATH)

    add_executable(example_preload86 example_preload.c)
    set_target_properties(example_preload86 PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    target_link_libraries(example_preload86 QBDI::X86::QBDI_static QBDIPreload::X86::QBDIPreload)

    find_package(QBDIX86_64 REQUIRED HINTS "${EXTRACT_DIRECTORY_X86_64}" NO_DEFAULT_PATH)
    find_package(QBDIPreloadX86_64 REQUIRED HINTS "${EXTRACT_DIRECTORY_X86_64}" NO_DEFAULT_PATH)

    add_executable(example_preload64 example_preload.c)
    target_link_libraries(example_preload64 QBDI::X86_64::QBDI_static QBDIPreload::X86_64::QBDIPreload)



