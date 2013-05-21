project (quicklist C)
include_directories (../cutest)

enable_testing()

INCLUDE_DIRECTORIES (${CUTEST_INCLUDE_DIR})
add_library (${PROJECT_NAME} quicklist.c quicklist.h)

add_executable (test_quicklist test_quicklist.c)
target_link_libraries (test_quicklist quicklist ${CUTEST_LIBRARIES})
add_test (quicklist test_quicklist)

set (QUICKLIST_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "QuickList headers")
set (QUICKLIST_LIBRARIES quicklist CACHE INTERNAL "QuickList libraries")
