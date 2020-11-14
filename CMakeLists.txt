cmake_minimum_required(VERSION 2.9)
project (clibs C)

FILE (GLOB LIB_HDR *.h)
add_library (${PROJECT_NAME} strings.c l10n.c strutil.c critbit.c selist.c format.c ${LIB_HDR})
add_library (cutest CuTest.c)
add_executable (test_clibs tests.c test_strings.c test_l10n.c test_selist.c test_strutil.c test_format.c test_critbit.c)
target_link_libraries (test_clibs cutest ${PROJECT_NAME})

enable_testing()
add_test (clibs test_clibs)
set (CLIBS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "clibs headers")
set (CLIBS_LIBRARIES ${PROJECT_NAME} CACHE INTERNAL "single-file libraries")

IF (MSVC)
    MACRO (MSVC_CRT_SECURE_NO_WARNINGS)
        FOREACH (target ${ARGN})
            SET_TARGET_PROPERTIES (${target} PROPERTIES
                COMPILE_DEFINITIONS _CRT_SECURE_NO_WARNINGS
            )
        ENDFOREACH (target)
    ENDMACRO (MSVC_CRT_SECURE_NO_WARNINGS)

    MACRO (MSVC_SET_WARNING_LEVEL level)
        IF(CMAKE_C_FLAGS MATCHES "/W[0-4]")
            STRING(REGEX REPLACE "/W[0-4]" "/W${level}" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
        ELSE()
            SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W${level}")
        ENDIF()
    ENDMACRO (MSVC_SET_WARNING_LEVEL)

    MSVC_SET_WARNING_LEVEL(3)
    MSVC_CRT_SECURE_NO_WARNINGS (cutest clibs test_clibs)
ENDIF (MSVC)
