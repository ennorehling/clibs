cmake_minimum_required(VERSION 2.4)
project (critbit C)
include_directories (../cutest)

add_library (critbit critbit.c)

add_executable (benchmark benchmark.c strtolh.c)
target_link_libraries (benchmark critbit)

add_executable (critbit_tests critbit_tests.c test_critbit.c)
target_link_libraries (critbit_tests critbit cutest)

enable_testing()
add_test (critbit critbit_tests)

find_package (MSVC QUIET)
if (${MSVC_FOUND})
    MSVC_CRT_SECURE_NO_WARNINGS (benchmark)
endif (${MSVC_FOUND})
