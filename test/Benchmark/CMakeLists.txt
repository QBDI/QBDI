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
