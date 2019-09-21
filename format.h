#pragma once

#include <stddef.h>

int format_replace(const char *input, const char *pattern,
    const char *str, char *output, size_t len);
    
char *format_list(int argc, const char *argv[],
    char *buffer, size_t len,
    const char *two, const char *start, const char *middle, const char *end);
