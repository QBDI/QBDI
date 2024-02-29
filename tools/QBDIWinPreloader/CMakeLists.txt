cmake_minimum_required(VERSION 3.15)
project(QBDIWinPreloader)

add_executable(QBDIWinPreloader)
target_sources(QBDIWinPreloader
               PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src/QBDIWinPreloader.c")

install(
  TARGETS QBDIWinPreloader
  EXPORT QBDIWinPreloader_targets
  RUNTIME DESTINATION bin)

if(PYQBDI_OUTPUT_DIRECTORY AND QBDI_TOOLS_PYQBDI)
  # If we build PyQBDI in order to create a wheel, we copy the binary
  # to the output directory in order to include it in the wheel.
  add_custom_command(
    TARGET QBDIWinPreloader
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:QBDIWinPreloader>
            "${PYQBDI_OUTPUT_DIRECTORY}/pyqbdiWinPreloader.exe")
endif()
