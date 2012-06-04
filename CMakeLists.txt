cmake_minimum_required(VERSION 2.4)
project (critbit C)
include_directories (../cutest)

add_library (${PROJECT_NAME} critbit.c)

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

set (CRITBIT_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "CritBit headers")
set (CRITBIT_LIBRARIES critbit CACHE INTERNAL "CritBit libraries")
