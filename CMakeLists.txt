PROJECT (quicklist C)

ENABLE_TESTING()

INCLUDE_DIRECTORIES (${CUTEST_INCLUDE_DIR})

ADD_LIBRARY (${PROJECT_NAME} quicklist.c quicklist.h)

ADD_EXECUTABLE (test_quicklist test_quicklist.c)
TARGET_LINK_LIBRARIES (test_quicklist quicklist ${CUTEST_LIBRARIES})
ADD_TEST (quicklist test_quicklist)

SET (QUICKLIST_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "QuickList headers")
SET (QUICKLIST_LIBRARIES quicklist CACHE INTERNAL "QuickList libraries")
