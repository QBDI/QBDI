add_executable(fibonacci_c fibonacci.c)
target_link_libraries(fibonacci_c QBDI)
add_signature(fibonacci_c)

if(QBDI_TOOLS_QBDIPRELOAD)
  add_library(tracer_preload_c SHARED tracer_preload.c)
  target_link_libraries(tracer_preload_c QBDI_static QBDIPreload)
endif()
