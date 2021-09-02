add_library(
  validator SHARED
  "${CMAKE_CURRENT_LIST_DIR}/instrumented.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/master.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/validatorengine.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/pipes.cpp")

if(QBDI_PLATFORM_LINUX OR QBDI_PLATFORM_ANDROID)
  target_sources(
    validator PRIVATE "${CMAKE_CURRENT_LIST_DIR}/linux_validator.cpp"
                      "${CMAKE_CURRENT_LIST_DIR}/linux_process.cpp")
elseif(QBDI_PLATFORM_OSX)
  target_sources(
    validator PRIVATE "${CMAKE_CURRENT_LIST_DIR}/darwin_validator.cpp"
                      "${CMAKE_CURRENT_LIST_DIR}/darwin_process.cpp")
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

# Also add build directory as include path for the mach_exc.h header
target_include_directories(
  validator
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
          $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src>
          $<INSTALL_INTERFACE:include>)
target_link_libraries(validator PRIVATE QBDIPreload QBDI_static
                                        spdlog_header_only)

set_target_properties(validator PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED
                                                           ON)
target_compile_options(validator
                       PRIVATE $<$<COMPILE_LANGUAGE:C>:${QBDI_COMMON_C_FLAGS}>)
target_compile_options(
  validator PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${QBDI_COMMON_CXX_FLAGS}>)
