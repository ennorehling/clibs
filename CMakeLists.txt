project (quicklist C)
include_directories (../cutest)

add_library (quicklist quicklist.c)

add_executable (quicklist_tests quicklist_tests.c test_quicklist.c)
target_link_libraries (quicklist_tests quicklist cutest)

enable_testing()
add_test (quicklist quicklist_tests)
