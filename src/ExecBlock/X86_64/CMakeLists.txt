# Add QBDI target
set(SOURCES "${CMAKE_CURRENT_LIST_DIR}/ExecBlock_X86_64.cpp")

if(QBDI_PLATFORM_OSX)
  if(QBDI_ARCH_X86_64)
    set(ASM_STUB_EXECBLOCK "${CMAKE_CURRENT_LIST_DIR}/osx_X86_64.s")
  else() # QBDI_ARCH_X86
    set(ASM_STUB_EXECBLOCK "${CMAKE_CURRENT_LIST_DIR}/osx_X86.s")
  endif()
  list(APPEND SOURCES "${ASM_STUB_EXECBLOCK}")

elseif(QBDI_PLATFORM_LINUX OR QBDI_PLATFORM_ANDROID)

  if(QBDI_ARCH_X86_64)
    set(ASM_STUB_EXECBLOCK "${CMAKE_CURRENT_LIST_DIR}/linux_X86_64.s")
  else() # QBDI_ARCH_X86
    set(ASM_STUB_EXECBLOCK "${CMAKE_CURRENT_LIST_DIR}/linux_X86.s")
  endif()
  list(APPEND SOURCES "${ASM_STUB_EXECBLOCK}")
  set_property(SOURCE "${ASM_STUB_EXECBLOCK}" PROPERTY LINK_FLAGS
                                                       "-Wl,-z,noexecstack")

elseif(QBDI_PLATFORM_WINDOWS)

  if(QBDI_ARCH_X86_64)
    list(APPEND SOURCES "${CMAKE_CURRENT_LIST_DIR}/windows_X86_64.asm")
  else() # QBDI_ARCH_X86
    list(APPEND SOURCES "${CMAKE_CURRENT_LIST_DIR}/windows_X86.asm")
  endif()

else()
  message(FATAL_ERROR "No stub for ${QBDI_PLATFORM} (${QBDI_ARCH})")
endif()

target_sources(QBDI_src INTERFACE "${SOURCES}")
