include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/tests)

add_executable(main_test c89atomic_basic.c)
add_executable(atomicity_test thread.c atomicity.c)
add_executable(core_test thread.c tests-core-atomic.c)

add_test(NAME main_test COMMAND main_test)
add_test(NAME core_test COMMAND core_test)
add_test(NAME atomicity_test COMMAND atomicity_test)
