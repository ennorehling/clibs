project (quicklist C)
include_directories (../cutest)

add_library (${PROJECT_NAME} quicklist.c)

add_executable (quicklist_tests quicklist_tests.c test_quicklist.c)
target_link_libraries (quicklist_tests quicklist cutest)

enable_testing()
add_test (quicklist quicklist_tests)

set (QUICKLIST_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "QuickList headers")
set (QUICKLIST_LIBRARIES quicklist CACHE INTERNAL "QuickList libraries")
