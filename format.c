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
            strncpy(output + plen + slen, pos + spat, 
                    sinp - plen - spat);
            memcpy(output + plen, str, slen);
            return true;
        }
    }
    return false;
}

const char *format_list(int argc, const char *argv[],
        char *buffer, size_t len,
        const char *two, 
        const char *start,
        const char *middle,
        const char *end)
{
    if (argc == 1) return argv[0];
    if (argc == 2) {
        if (str_replace(two, "{0}", argv[0], buffer, len)) {
            if (str_replace(buffer, "{1}", argv[1], buffer, len)) {
                return buffer;
            }
        }
    }
    else {
    }
    return NULL;
}

int main(int argc, const char * argv[]) {
    char buffer[128];
    const char *str = format_list(argc-1, argv+1, buffer, sizeof(buffer),
                     "{0} and {1}",
                     "{0}, {1}",
                     "{0}, {1}",
                     "{0} and {1}");
    return puts(str ? str : "error");
}
    
