cmake_minimum_required(VERSION 2.6)
project (clibs C)

FILE (GLOB LIB_HDR *.h)
add_library (${PROJECT_NAME} strutil.c critbit.c fastlist.c ${LIB_HDR})

add_executable (test_clibs tests.c CuTest.c test_fastlist.c test_strutil.c test_critbit.c)
target_link_libraries (test_clibs ${PROJECT_NAME})

enable_testing()
add_test (clibs test_clibs)
set (CLIBS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "clibs headers")
set (CLIBS_LIBRARIES ${PROJECT_NAME} CACHE INTERNAL "single-file libraries")

IF (MSVC)
    FIND_PACKAGE (MSVC MODULE)
    MSVC_SET_WARNING_LEVEL(3)
    MSVC_CRT_SECURE_NO_WARNINGS (test_clibs)
ENDIF (MSVC)
