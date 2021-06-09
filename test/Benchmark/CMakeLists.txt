include(FetchContent)

FetchContent_Declare(
  sha256_lib
  URL "https://github.com/aguinet/sha256_literal/archive/d7017a7b4bbc30bc93fb8bd4cf54555986d25ef0.zip"
  URL_HASH
    "SHA256=7f7c3771764829ec74ee3f9cfc9df6250051f92d6e7f2e1e11f06a0b48938b2c"
  DOWNLOAD_DIR "${CMAKE_SOURCE_DIR}/third-party/sha256_lib-download")

FetchContent_GetProperties(sha256_lib)
if(NOT sha256_lib_POPULATED)
  FetchContent_Populate(sha256_lib)
endif()

add_custom_command(
  OUTPUT "${sha256_lib_SOURCE_DIR}/sha256_impl.cpp"
  COMMAND
    ${CMAKE_COMMAND} -E create_symlink "${sha256_lib_SOURCE_DIR}/sha256.cpp"
    "${sha256_lib_SOURCE_DIR}/sha256_impl.cpp")

target_include_directories(QBDIBenchmark PRIVATE "${sha256_lib_SOURCE_DIR}")

# set sources
set(SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Fibonacci.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SHA256.cpp"
    "${sha256_lib_SOURCE_DIR}/sha256_impl.cpp")

target_sources(QBDIBenchmark PUBLIC "${SOURCES}")
