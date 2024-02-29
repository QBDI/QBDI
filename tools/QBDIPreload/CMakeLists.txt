add_library(QBDIPreload STATIC)
add_library(QBDIPreload::QBDIPreload ALIAS QBDIPreload)

include("${CMAKE_CURRENT_LIST_DIR}/src/CMakeLists.txt")
target_compile_options(QBDIPreload PRIVATE $<$<COMPILE_LANGUAGE:C>:
                                           ${QBDI_COMMON_C_FLAGS}>)
target_compile_definitions(QBDIPreload PRIVATE ${QBDI_COMMON_DEFINITION})

# Also add build directory as include path for the mach_exc.h header
target_include_directories(
  QBDIPreload
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../../include>
          $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../../include-static>
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
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
