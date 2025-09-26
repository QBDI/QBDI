# Add QBDI target
target_sources(
  QBDI_src
  INTERFACE "${CMAKE_CURRENT_LIST_DIR}/InstAnalysis.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/LogSys.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/Memory.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/StackSwitch.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/String.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/Version.cpp")

if(QBDI_PLATFORM_ANDROID OR QBDI_PLATFORM_LINUX)
  target_sources(
    QBDI_src INTERFACE "${CMAKE_CURRENT_LIST_DIR}/Memory_linux.cpp"
                       "${CMAKE_CURRENT_LIST_DIR}/System_generic.cpp")
elseif(QBDI_PLATFORM_OSX OR QBDI_PLATFORM_IOS)
  target_sources(QBDI_src INTERFACE "${CMAKE_CURRENT_LIST_DIR}/Memory_osx.cpp")
  if(NOT QBDI_ARCH_AARCH64)
    target_sources(QBDI_src
                   INTERFACE "${CMAKE_CURRENT_LIST_DIR}/System_generic.cpp")
  endif()
elseif(QBDI_PLATFORM_WINDOWS)
  target_sources(
    QBDI_src INTERFACE "${CMAKE_CURRENT_LIST_DIR}/Memory_windows.cpp"
                       "${CMAKE_CURRENT_LIST_DIR}/System_generic.cpp")
endif()

if(QBDI_ARCH_X86 OR QBDI_ARCH_X86_64)
  include("${CMAKE_CURRENT_LIST_DIR}/X86_64/CMakeLists.txt")
elseif(QBDI_ARCH_ARM)
  include("${CMAKE_CURRENT_LIST_DIR}/ARM/CMakeLists.txt")
elseif(QBDI_ARCH_AARCH64)
  include("${CMAKE_CURRENT_LIST_DIR}/AARCH64/CMakeLists.txt")
endif()
