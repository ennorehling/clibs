project (critbit C)
include_directories (../cutest)

add_library (critbit critbit.c)

add_executable (benchmark benchmark.c strtolh.c)
target_link_libraries (benchmark critbit)

add_executable (critbit_tests critbit_tests.c test_critbit.c)
target_link_libraries (critbit_tests critbit cutest)

enable_testing()
add_test (critbit critbit_tests)
