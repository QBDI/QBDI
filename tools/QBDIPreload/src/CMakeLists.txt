if(QBDI_PLATFORM_LINUX)
  target_sources(QBDIPreload
                 PRIVATE "${CMAKE_CURRENT_LIST_DIR}/linux_preload.c")
elseif(QBDI_PLATFORM_OSX)
  # Generate MIG interface at config time in the build directory
  execute_process(
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND mig ${CMAKE_OSX_SYSROOT}/usr/include/mach/mach_exc.defs)
  target_sources(
    QBDIPreload
    PRIVATE "${CMAKE_CURRENT_LIST_DIR}/darwin_preload.c"
            "${CMAKE_CURRENT_LIST_DIR}/darwin_exceptd.c" "mach_excServer.c")
else()
  message(FATAL_ERROR "No QBDIPreload for platform ${QBDI_PLATFORM}")
endif()

if(QBDI_ARCH_X86_64)
  include("${CMAKE_CURRENT_LIST_DIR}/X86_64/CMakeLists.txt")
elseif(QBDI_ARCH_X86)
  include("${CMAKE_CURRENT_LIST_DIR}/X86/CMakeLists.txt")
elseif(QBDI_ARCH_ARM)
  include("${CMAKE_CURRENT_LIST_DIR}/ARM/CMakeLists.txt")
elseif(QBDI_ARCH_AARCH64)
  include("${CMAKE_CURRENT_LIST_DIR}/AARCH64/CMakeLists.txt")
else()
  message(FATAL_ERROR "No stub for architecture ${QBDI_ARCH}")
endif()
