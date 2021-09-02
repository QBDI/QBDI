# Add QBDI target
set(SOURCES "${CMAKE_CURRENT_LIST_DIR}/ExecBlock_X86_64.cpp")

if(QBDI_PLATFORM_OSX
   OR QBDI_PLATFORM_LINUX
   OR QBDI_PLATFORM_ANDROID)

  if(QBDI_ARCH_X86_64)
    list(APPEND SOURCES "${CMAKE_CURRENT_LIST_DIR}/linux-osx-X86_64.s")
  else() # QBDI_ARCH_X86
    list(APPEND SOURCES "${CMAKE_CURRENT_LIST_DIR}/linux-osx-X86.s")
  endif()

elseif(QBDI_PLATFORM_WINDOWS)

  if(QBDI_ARCH_X86_64)
    list(APPEND SOURCES "${CMAKE_CURRENT_LIST_DIR}/windows-X86_64.asm")
  else() # QBDI_ARCH_X86
    list(APPEND SOURCES "${CMAKE_CURRENT_LIST_DIR}/windows-X86.asm")
  endif()

else()
  message(FATAL_ERROR "No stub for ${QBDI_PLATFORM} (${QBDI_ARCH})")
endif()

target_sources(QBDI_src INTERFACE "${SOURCES}")
