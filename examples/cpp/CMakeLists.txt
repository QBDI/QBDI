add_executable(fibonacci_cpp fibonacci.cpp)
target_link_libraries(fibonacci_cpp QBDI)
set_target_properties(fibonacci_cpp PROPERTIES CXX_STANDARD 11
                                               CXX_STANDARD_REQUIRED ON)
add_signature(fibonacci_cpp)

if(QBDI_TOOLS_QBDIPRELOAD)
  add_library(tracer_preload_cpp SHARED tracer_preload.cpp)
  target_link_libraries(tracer_preload_cpp QBDI_static QBDIPreload)
  set_target_properties(tracer_preload_cpp PROPERTIES CXX_STANDARD 11
                                                      CXX_STANDARD_REQUIRED ON)
endif()
