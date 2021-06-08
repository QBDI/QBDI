set(SOURCES "instrumented.cpp" "master.cpp" "validatorengine.cpp" "pipes.cpp")

if(QBDI_PLATFORM_LINUX OR QBDI_PLATFORM_ANDROID)
  set(SOURCES ${SOURCES} "linux_validator.cpp" "linux_process.cpp")
elseif(QBDI_PLATFORM_OSX)
  set(SOURCES ${SOURCES} "darwin_validator.cpp" "darwin_process.cpp")
endif()

add_library(validator SHARED ${SOURCES})

# Also add build directory as include path for the mach_exc.h header
target_include_directories(
  validator
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src>
          $<INSTALL_INTERFACE:include>)
target_link_libraries(validator PRIVATE QBDIPreload QBDI_static
                                        spdlog_header_only)

set_target_properties(validator PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED
                                                           ON)
