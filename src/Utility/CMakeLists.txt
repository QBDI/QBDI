# Add QBDI target
target_sources(
  QBDI_src
  INTERFACE "${CMAKE_CURRENT_LIST_DIR}/InstAnalysis.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/LogSys.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/Memory.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/Memory_${QBDI_PLATFORM}.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/String.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/System.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/Version.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/memory_ostream.cpp")

if(QBDI_PLATFORM_IOS)
  target_sources(
    QBDI_src INTERFACE "${CMAKE_CURRENT_LIST_DIR}/System_iOS.cpp"
                       "${CMAKE_CURRENT_LIST_DIR}/server-iOS-jit-user.c")
else()
  target_sources(QBDI_src
                 INTERFACE "${CMAKE_CURRENT_LIST_DIR}/System_generic.cpp")
endif()

if(QBDI_ARCH_X86 OR QBDI_ARCH_X86_64)
  include("${CMAKE_CURRENT_LIST_DIR}/X86_64/CMakeLists.txt")
elseif(QBDI_ARCH_ARM)
  include("${CMAKE_CURRENT_LIST_DIR}/ARM/CMakeLists.txt")
elseif(QBDI_ARCH_AARCH64)
  include("${CMAKE_CURRENT_LIST_DIR}/AARCH64/CMakeLists.txt")
endif()
