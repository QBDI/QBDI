# Add QBDI target
set(SOURCES "${CMAKE_CURRENT_LIST_DIR}/ExecBroker.cpp")

if(QBDI_ARCH_X86 OR QBDI_ARCH_X86_64)
  target_sources(
    QBDI_src INTERFACE "${CMAKE_CURRENT_LIST_DIR}/X86_64/ExecBroker_X86_64.cpp")
elseif(QBDI_ARCH_ARM)
  target_sources(QBDI_src
                 INTERFACE "${CMAKE_CURRENT_LIST_DIR}/ARM/ExecBroker_ARM.cpp")
elseif(QBDI_ARCH_AARCH64)
  target_sources(
    QBDI_src
    INTERFACE "${CMAKE_CURRENT_LIST_DIR}/AARCH64/ExecBroker_AARCH64.cpp")
else()
  message(FATAL_ERROR "Missing ExecBroker for ${QBDI_ARCH}")
endif()

target_sources(QBDI_src INTERFACE "${SOURCES}")
