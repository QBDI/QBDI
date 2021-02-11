add_executable(mnemonicFilter mnemonicFilter.cpp)
target_link_libraries(mnemonicFilter QBDI)
set_target_properties(mnemonicFilter PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON)
add_signature(mnemonicFilter)

add_executable(modules_c modules.c)
target_link_libraries(modules_c QBDI)
add_signature(modules_c)

add_executable(modules_cpp modules.cpp)
target_link_libraries(modules_cpp QBDI)
set_target_properties(modules_cpp PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON)
add_signature(modules_cpp)

add_executable(thedude_c thedude.c)
target_link_libraries(thedude_c QBDI)
add_signature(thedude_c)

add_executable(thedude_cpp thedude.cpp)
target_link_libraries(thedude_cpp QBDI)
set_target_properties(thedude_cpp PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON)
add_signature(thedude_cpp)

add_executable(cryptolock_cpp cryptolock.cpp)
target_link_libraries(cryptolock_cpp QBDI)
set_target_properties(cryptolock_cpp PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON)
add_signature(cryptolock_cpp)

add_executable(cryptolock_c cryptolock.c)
target_link_libraries(cryptolock_c QBDI)
add_signature(cryptolock_c)

add_subdirectory(c)
add_subdirectory(cpp)
