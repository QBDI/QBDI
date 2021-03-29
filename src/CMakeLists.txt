include("${CMAKE_CURRENT_LIST_DIR}/Engine/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/ExecBlock/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/ExecBroker/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/Patch/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/Utility/CMakeLists.txt")

target_sources(QBDI_shared_src
               INTERFACE "${CMAKE_CURRENT_LIST_DIR}/fridaStubs.cpp")

if(QBDI_PLATFORM_WINDOWS)
  target_sources(QBDI_shared_src
                 INTERFACE "${CMAKE_CURRENT_LIST_DIR}/windowsDllMain.cpp")
endif()
