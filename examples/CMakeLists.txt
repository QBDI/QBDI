add_executable(mnemonicFilter mnemonicFilter.cpp)
target_link_libraries(mnemonicFilter QBDI)
add_signature(mnemonicFilter)

add_executable(modules_c modules.c)
target_link_libraries(modules_c QBDI)
add_signature(modules_c)

add_executable(modules_cpp modules.cpp)
target_link_libraries(modules_cpp QBDI)
add_signature(modules_cpp)

add_executable(fibonacci_cpp fibonacci.cpp)
target_link_libraries(fibonacci_cpp QBDI)
add_signature(fibonacci_cpp)

add_executable(fibonacci_c fibonacci.c)
target_link_libraries(fibonacci_c QBDI)
add_signature(fibonacci_c)

add_executable(thedude_c thedude.c)
target_link_libraries(thedude_c QBDI)
add_signature(thedude_c)

add_executable(thedude_cpp thedude.cpp)
target_link_libraries(thedude_cpp QBDI)
add_signature(thedude_cpp)

add_executable(cryptolock_cpp cryptolock.cpp)
target_link_libraries(cryptolock_cpp QBDI)
add_signature(cryptolock_cpp)

add_executable(cryptolock_c cryptolock.c)
target_link_libraries(cryptolock_c QBDI)
add_signature(cryptolock_c)
