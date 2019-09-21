#pragma once

#include <stddef.h>

char *format_list(int argc, const char *argv[],
    char *buffer, size_t len,
    const char *two, const char *start, const char *middle, const char *end);
