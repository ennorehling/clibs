cmake_minimum_required(VERSION 2.6)
project (libs C)

FILE (GLOB LIB_HDR *.h)
add_library (${PROJECT_NAME} critbit.c quicklist.c ${LIB_HDR})

add_executable (test_quicklist CuTest.c test_quicklist.c)
target_link_libraries (test_quicklist ${PROJECT_NAME})

add_executable (test_critbit CuTest.c test_critbit.c)
target_link_libraries (test_critbit ${PROJECT_NAME})

enable_testing()
add_test (critbit test_critbit)
add_test (quicklist test_quicklist)

#set (CRITBIT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "CritBit headers")
#set (CRITBIT_LIBRARIES critbit CACHE INTERNAL "CritBit libraries")

