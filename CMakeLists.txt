cmake_minimum_required(VERSION 2.9)
project (clibs C)

if (MSVC)
  add_compile_options(/WX)
  if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  else()
    add_compile_options(/W4)
  endif()
endif (MSVC)

include (CheckFunctionExists)
include (CheckLibraryExists)
check_function_exists(strdup HAVE_STRDUP)
check_function_exists(strlcpy HAVE_STRLCPY)
check_function_exists(strlcat HAVE_STRLCAT)
if (HAVE_STRDUP)
  add_compile_definitions(HAVE_STRDUP)
else (HAVE_STRDUP)
  check_library_exists(bsd strdup "" HAVE_LIBBSD)
endif (HAVE_STRDUP)
if (HAVE_STRLCPY)
  add_compile_definitions(HAVE_STRLCPY)
else (HAVE_STRLCPY)
  check_library_exists(bsd strlcpy "" HAVE_LIBBSD)
endif(HAVE_STRLCPY)
if (HAVE_STRLCAT)
  add_compile_definitions(HAVE_STRLCAT)
else (HAVE_STRLCAT)
  check_library_exists(bsd strlcat "" HAVE_LIBBSD)
endif(HAVE_STRLCAT)

if (HAVE_LIBBSD)
  add_compile_definitions(HAVE_LIBBSD)
  add_compile_definitions(HAVE_STRDUP)
  add_compile_definitions(HAVE_STRLCPY)
  add_compile_definitions(HAVE_STRLCAT)
endif (HAVE_LIBBSD)

file (GLOB LIB_HDR *.h)
add_library (${PROJECT_NAME} strings.c l10n.c strutil.c critbit.c selist.c format.c ${LIB_HDR})
add_library (cutest CuTest.c)
add_executable (test_clibs tests.c test_strings.c test_l10n.c test_selist.c test_strutil.c test_format.c test_critbit.c)
target_link_libraries (test_clibs cutest ${PROJECT_NAME})
if (HAVE_LIBBSD)
	target_link_libraries (test_clibs bsd)
endif (HAVE_LIBBSD)
enable_testing()
add_test (clibs test_clibs)
set (CLIBS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "clibs headers")
set (CLIBS_LIBRARIES ${PROJECT_NAME} CACHE INTERNAL "single-file libraries")
