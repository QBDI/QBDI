include(FetchContent)

FetchContent_Populate(
  sha256_lib
  URL "https://github.com/aguinet/sha256_literal/archive/d7017a7b4bbc30bc93fb8bd4cf54555986d25ef0.zip"
  URL_HASH
    "SHA256=7f7c3771764829ec74ee3f9cfc9df6250051f92d6e7f2e1e11f06a0b48938b2c"
  DOWNLOAD_DIR "${QBDI_THIRD_PARTY_DIRECTORY}/sha256_lib-download"
  SOURCE_DIR "${FETCHCONTENT_BASE_DIR}/sha256_lib-src"
  BINARY_DIR "${FETCHCONTENT_BASE_DIR}/sha256_lib-build"
  SUBBUILD_DIR "${FETCHCONTENT_BASE_DIR}/sha256_lib-subbuild"
  QUIET)

add_custom_command(
  OUTPUT "${sha256_lib_SOURCE_DIR}/sha256_impl.cpp"
  COMMAND
    ${CMAKE_COMMAND} -E create_symlink "${sha256_lib_SOURCE_DIR}/sha256.cpp"
    "${sha256_lib_SOURCE_DIR}/sha256_impl.cpp")

target_include_directories(QBDIBenchmark PRIVATE "${sha256_lib_SOURCE_DIR}")

# set sources
target_sources(
  QBDIBenchmark
  PRIVATE "${CMAKE_CURRENT_LIST_DIR}/Fibonacci.cpp"
          "${CMAKE_CURRENT_LIST_DIR}/SHA256.cpp"
          "${sha256_lib_SOURCE_DIR}/sha256_impl.cpp")
