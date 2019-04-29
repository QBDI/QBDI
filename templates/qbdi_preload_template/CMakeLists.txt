cmake_minimum_required (VERSION 3.2)
project(QBDITemplate)

find_package(QBDI REQUIRED)
find_package(QBDIPreload REQUIRED)

add_library(qbdi_tracer SHARED qbdi_preload_template.c)
target_link_libraries(qbdi_tracer QBDI::QBDI QBDIPreload::QBDIPreload)
