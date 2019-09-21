#include <string.h>
#include <stdio.h>
#include <stdbool.h>

bool str_replace(const char *input, const char *pattern, 
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
	    if (plen > 0) {
		// copy text before the pattern:
            memcpy(output, input, plen);
	    }
	    // copy text after the pattern, leave room for str:
            memcpy(output + plen + slen, pos + spat, 
                    1 + sinp - plen - spat);
            memcpy(output + plen, str, slen);
            return true;
        }
    }
    return false;
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
        if (str_replace(two, "{0}", argv[0], buffer, len)) {
            if (str_replace(buffer, "{1}", argv[1], buffer, len)) {
                return buffer;
            }
        }
    }
    else if (str_replace(start, "{0}", argv[0], buffer, len)) {
        int i;
        for (i = 1; i < argc - 2; ++i) {
            if (!str_replace(buffer, "{1}", middle, buffer, len)) goto fail;
            if (!str_replace(buffer, "{0}", argv[i], buffer, len)) goto fail;
        }
        if (!str_replace(buffer, "{1}", end, buffer, len)) goto fail;
        if (!str_replace(buffer, "{0}", argv[argc - 2], buffer, len)) goto fail;
        if (str_replace(buffer, "{1}", argv[argc - 1], buffer, len)) {
            return buffer;
        }
    }
    fail:
    return NULL;
}
