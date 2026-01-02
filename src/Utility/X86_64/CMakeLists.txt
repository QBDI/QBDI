set(SOURCES "${CMAKE_CURRENT_LIST_DIR}/InstAnalysis_X86_64.cpp")

if(QBDI_PLATFORM_OSX)
  if(QBDI_ARCH_X86_64)
    set(ASM_STUB_UTILITY "${CMAKE_CURRENT_LIST_DIR}/osx-X86_64.s")
  else() # QBDI_ARCH_X86
    set(ASM_STUB_UTILITY "${CMAKE_CURRENT_LIST_DIR}/osx-X86.s")
  endif()
  list(APPEND SOURCES "${ASM_STUB_UTILITY}")

elseif(QBDI_PLATFORM_LINUX OR QBDI_PLATFORM_ANDROID)

  if(QBDI_ARCH_X86_64)
    set(ASM_STUB_UTILITY "${CMAKE_CURRENT_LIST_DIR}/linux-X86_64.s")
  else() # QBDI_ARCH_X86
    set(ASM_STUB_UTILITY "${CMAKE_CURRENT_LIST_DIR}/linux-X86.s")
  endif()
  list(APPEND SOURCES "${ASM_STUB_UTILITY}")
  set_property(SOURCE "${ASM_STUB_UTILITY}" PROPERTY LINK_FLAGS
                                                     "-Wl,-z,noexecstack")

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
