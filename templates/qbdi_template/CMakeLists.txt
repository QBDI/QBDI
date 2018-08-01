cmake_minimum_required (VERSION 3.2)
project(QBDITemplate)

find_package(QBDI REQUIRED)

add_executable(qbdi_template qbdi_template.c)
target_link_libraries(qbdi_template QBDI::QBDI)
