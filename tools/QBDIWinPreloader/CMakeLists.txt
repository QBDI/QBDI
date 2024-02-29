cmake_minimum_required(VERSION 3.15)
project(QBDIWinPreloader)

add_executable(QBDIWinPreloader)
target_sources(QBDIWinPreloader
               PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src/QBDIWinPreloader.c")

install(
  TARGETS QBDIWinPreloader
  EXPORT QBDIWinPreloader_targets
  RUNTIME DESTINATION bin)
