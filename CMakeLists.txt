cmake_minimum_required(VERSION 2.6)
project (clibs C)

FILE (GLOB LIB_HDR *.h)
add_library (${PROJECT_NAME} strutil.c critbit.c quicklist.c ${LIB_HDR})

add_executable (tests tests.c CuTest.c test_quicklist.c test_strutil.c test_critbit.c)
target_link_libraries (tests ${PROJECT_NAME})

enable_testing()
add_test (clibs tests)
set (CLIBS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "CritBit headers")
set (CLIBS_LIBRARIES ${PROJECT_NAME} CACHE INTERNAL "single-file libraries")

