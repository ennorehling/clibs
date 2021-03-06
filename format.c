#include <string.h>
#include <stdio.h>

int format_replace(const char *input, const char *pattern, 
        const char *str, char *output, size_t len)
{
    size_t slen = strlen(str);
    size_t spat = strlen(pattern);
    size_t sinp = strlen(input);
    size_t req = sinp + slen - spat;
    if (req < len) {
        char *pos = strstr(input, pattern);
        if (pos) {
            size_t plen = pos - input;
            // copy text after the pattern, leave room for str:
            memmove(output + plen + slen, pos + spat,
                1 + sinp - plen - spat);
            // replace the pattern:
            memmove(output + plen, str, slen);
            if (plen > 0 && output != input) {
		        // copy text before the pattern:
                memcpy(output, input, plen);
	        }
            return 1;
        }
    }
    return 0;
}

char *format_list(int argc, const char *argv[],
    char *buffer, size_t len,
    const char *two,
    const char *start,
    const char *middle,
    const char *end)
{
    if (argc == 1) {
        size_t slen = strlen(argv[0]);
        if (slen < len) {
            memcpy(buffer, argv[0], slen + 1);
            return buffer;
        }
    }
    else if (argc == 2) {
        if (format_replace(two, "{0}", argv[0], buffer, len)) {
            if (format_replace(buffer, "{1}", argv[1], buffer, len)) {
                return buffer;
            }
        }
    }
    else if (format_replace(start, "{0}", argv[0], buffer, len)) {
        int i;
        for (i = 1; i < argc - 2; ++i) {
            if (!format_replace(buffer, "{1}", middle, buffer, len)) return NULL;
            if (!format_replace(buffer, "{0}", argv[i], buffer, len)) return NULL;
        }
        if (!format_replace(buffer, "{1}", end, buffer, len)) return NULL;
        if (!format_replace(buffer, "{0}", argv[argc - 2], buffer, len)) return NULL;
        if (format_replace(buffer, "{1}", argv[argc - 1], buffer, len)) {
            return buffer;
        }
    }
    return NULL;
}
