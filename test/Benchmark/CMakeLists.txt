# Use external SHA256 implementation
ExternalProject_Add(
  SHA256_lib
  GIT_REPOSITORY "https://github.com/aguinet/sha256_literal.git"
  GIT_PROGRESS "true"
  GIT_TAG "d7017a7b4bbc30bc93fb8bd4cf54555986d25ef0"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND "")

ExternalProject_Get_Property(SHA256_lib SOURCE_DIR)
set(SHA256_lib_SOURCE_DIR "${SOURCE_DIR}")

add_custom_command(
  OUTPUT "${SHA256_lib_SOURCE_DIR}/sha256_impl.cpp"
  DEPENDS SHA256_lib
  COMMAND
    ${CMAKE_COMMAND} -E create_symlink "${SHA256_lib_SOURCE_DIR}/sha256.cpp"
    "${SHA256_lib_SOURCE_DIR}/sha256_impl.cpp")

add_dependencies(QBDIBenchmark SHA256_lib)

target_include_directories(QBDIBenchmark PRIVATE "${SHA256_lib_SOURCE_DIR}")

# set sources
set(SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Fibonacci.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SHA256.cpp"
    "${SHA256_lib_SOURCE_DIR}/sha256_impl.cpp")

target_sources(QBDIBenchmark PUBLIC "${SOURCES}")
