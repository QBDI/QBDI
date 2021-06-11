set(SOURCES "")

if(QBDI_PLATFORM_LINUX OR QBDI_PLATFORM_ANDROID)
  set(SOURCES ${SOURCES} "src/linux_preload.c")
elseif(QBDI_PLATFORM_OSX)
  # Generate MIG interface at config time in the build directory
  execute_process(
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND mig ${CMAKE_OSX_SYSROOT}/usr/include/mach/mach_exc.defs)
  set(SOURCES ${SOURCES} "src/darwin_preload.c" "src/darwin_exceptd.c"
              "mach_excServer.c")
endif()

add_library(QBDIPreload STATIC ${SOURCES})

# Also add build directory as include path for the mach_exc.h header
target_include_directories(
  QBDIPreload
  PRIVATE $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
          $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include/QBDI>
          $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
          $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/QBDI>
          $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
          $<INSTALL_INTERFACE:include>)

target_include_directories(
  QBDIPreload INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

install(
  TARGETS QBDIPreload
  EXPORT QBDIPreload_targets
  ARCHIVE DESTINATION lib)

install(FILES include/QBDIPreload.h DESTINATION include/)

install(
  EXPORT QBDIPreload_targets
  FILE QBDIPreloadConfig.cmake
  NAMESPACE QBDIPreload::
  DESTINATION ${PRELOAD_RESOURCES_PREFIX}/cmake)

install(
  EXPORT QBDIPreload_targets
  FILE QBDIPreload${QBDI_ARCH}Config.cmake
  NAMESPACE QBDIPreload::${QBDI_ARCH}::
  DESTINATION ${PRELOAD_RESOURCES_PREFIX}/cmake)
