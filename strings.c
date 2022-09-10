#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#define HAVE__ITOA
#endif
#include "strings.h"

/* libc includes */
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_LIBBSD
#include <bsd/string.h>
#else
#include <string.h>
#endif

const char* str_itoa_r(int val, char *buf)
{
#ifdef HAVE__ITOA
    return _itoa(val, buf, 10);
#else
    snprintf(buf, 12, "%d", val);
    return buf;
#endif
}

const char *str_itoa(int n)
{
    static char buf[12];
#ifdef HAVE__ITOA
    return _itoa(n, buf, 10);
#else
    return str_itoa_r(n, buf);
#endif
}

size_t str_strlcpy(char *dst, const char *src, size_t len)
{
#ifdef HAVE_STRLCPY
    return strlcpy(dst, src, len);
#else
    register char *d = dst;
    register const char *s = src;
    register size_t n = len;

    assert(src);
    assert(dst);
    /* Copy as many bytes as will fit */
    if (n != 0 && --n != 0) {
        do {
            if ((*d++ = *s++) == 0)
                break;
        } while (--n != 0);
    }

    /* Not enough room in dst, add NUL and traverse rest of src */
    if (n == 0) {
        if (len != 0) {
            *d = '\0'; /* NUL-terminate dst */
        }
        return (s - src) + strlen(s); /* count does not include NUL */
    }

    return (s - src - 1);         /* count does not include NUL */
#endif
}

size_t str_strlcat(char *dst, const char *src, size_t len)
{
#ifdef HAVE_STRLCAT
    return strlcat(dst, src, len);
#else
    register char *d = dst;
    register const char *s = src;
    register size_t n = len;
    size_t dlen;

    /* Find the end of dst and adjust bytes left but don't go past end */
    while (*d != '\0' && n-- != 0)
        d++;
    dlen = d - dst;
    n = len - dlen;

    if (n == 0)
        return (dlen + strlen(s));
    while (*s != '\0') {
        if (n != 1) {
            *d++ = *s;
            n--;
        }
        s++;
    }
    *d = '\0';

    return (dlen + (s - src));    /* count does not include NUL */
#endif
}

size_t str_slprintf(char * dst, size_t size, const char * format, ...)
{
    va_list args;
    int result;

    va_start(args, format);
    result = vsnprintf(dst, size, format, args);
    va_end(args);
    if (result < 0 || result >= (int)size) {
        dst[size - 1] = '\0';
        return size;
    }

    return (size_t)result;
}

void str_replace(char *buffer, size_t size, const char *tmpl, const char *var,
    const char *value)
{
    size_t val_len = strlen(value);
    size_t var_len = strlen(var);
    char *s = buffer;
    while (buffer + size > s) {
        char *p = strstr(tmpl, var);
        size_t len;
        if (p) {
            len = p - tmpl;
        }
        else {
            len = strlen(tmpl);
        }
        if (len < size) {
            memmove(s, tmpl, len);
            tmpl += len;
            s += len;
            size -= len;
            if (p && val_len < size) {
                tmpl += var_len;
                memmove(s, value, val_len);
                s += val_len;
                size -= val_len;
            }
        }
        if (!p) {
            break;
        }
    }
    *s = 0;
}

int str_hash(const char *s)
{
    int key = 0;
    assert(s);
    while (*s) {
        key = key * 37 + *s++;
    }
    return key & 0x7FFFFFFF;
}

unsigned int jenkins_hash(unsigned int a)
{
    a = (a + 0x7ed55d16) + (a << 12);
    a = (a ^ 0xc761c23c) ^ (a >> 19);
    a = (a + 0x165667b1) + (a << 5);
    a = (a + 0xd3a2646c) ^ (a << 9);
    a = (a + 0xfd7046c5) + (a << 3);
    a = (a ^ 0xb55a4f09) ^ (a >> 16);
    return a;
}

unsigned int wang_hash(unsigned int a)
{
    a = ~a + (a << 15);           /*  a = (a << 15) - a - 1; */
    a = a ^ (a >> 12);
    a = a + (a << 2);
    a = a ^ (a >> 4);
    a = a * 2057;                 /*  a = (a + (a << 3)) + (a << 11); */
    a = a ^ (a >> 16);
    return a;
}

char *str_strdup(const char *s) {
    if (s == NULL) return NULL;
#ifdef HAVE_STRDUP
    return strdup(s);
#elif defined(_MSC_VER)
    return _strdup(s);
#else
    size_t len = strlen(s);
    char *dup = malloc(len + 1);
    if (dup) {
        memcpy(dup, s, len + 1);
    }
    return dup;
#endif
}

void sbs_printf(struct sbstring *sbs, const char *format, ...)
{
    size_t size = (sbs->end - sbs->begin);

    if (size < sbs->size) {
        va_list argp;
        va_start(argp, format);
        int bytes = vsnprintf(sbs->end, sbs->size - size, format, argp);
        if (bytes > 0) {
            if ((size_t)bytes >= sbs->size - size) {
                bytes = (int)(sbs->size - size - 1);
                /* terminate truncated output */
                sbs->end[bytes] = '\0';
            }
            sbs->end += bytes;
        }
        va_end(argp);
    }
}

void sbs_init(struct sbstring *sbs, char *buffer, size_t size)
{
    assert(sbs);
    assert(size > 0);
    sbs->begin = buffer;
    sbs->size = size;
    sbs->end = buffer;
    buffer[0] = '\0';
}

void sbs_adopt(struct sbstring *sbs, char *buffer, size_t size)
{
    size_t len = strlen(buffer);
    assert(sbs);
    assert(size > len);
    sbs->begin = buffer;
    sbs->size = size;
    sbs->end = buffer + len;
}

void sbs_strncat(struct sbstring *sbs, const char *str, size_t size)
{
    size_t len;
    assert(sbs);
    len = sbs->size - (sbs->end - sbs->begin) - 1;
    if (len < size) {
        size = len;
    }
    memcpy(sbs->end, str, size);
    sbs->end[size] = '\0';
    sbs->end += size;
}

void sbs_strcat(struct sbstring *sbs, const char *str)
{
    size_t len;
    assert(sbs);
    len = sbs->size - (sbs->end - sbs->begin);
    str_strlcpy(sbs->end, str, len);
    sbs->end += strlen(sbs->end);
    assert(sbs->begin + sbs->size >= sbs->end);
}

void sbs_substr(sbstring *sbs, ptrdiff_t pos, size_t len)
{
    if (pos > sbs->end - sbs->begin) {
        /* starting past end of string, do nothing */
        sbs->end = sbs->begin;
    }
    if (pos >= 0) {
        size_t sz = sbs->end - (sbs->begin + pos);
        if (len > sz) len = sz;
        if (len - pos > 0) {
            memmove(sbs->begin, sbs->begin + pos, len);
        }
        else {
            memcpy(sbs->begin, sbs->begin + pos, len);
        }
        sbs->end = sbs->begin + len;
        sbs->end[0] = '\0';
    }
}

size_t sbs_length(const struct sbstring *sbs)
{
    assert(sbs->begin + sbs->size >= sbs->end);
    return sbs->end - sbs->begin;
}

char *str_unescape(char *str) {
    char *read = str, *write = str;
    while (*read) {
        char * pos = strchr(read, '\\');
        if (pos >= read) {
            size_t len = (size_t)(pos - read);
            memmove(write, read, len);
            write += len;
            read += (len + 1);
            switch (read[0]) {
            case 'r':
                *write++ = '\r';
                break;
            case 'n':
                *write++ = '\n';
                break;
            case 't':
                *write++ = '\t';
                break;
            default: 
                *write++ = read[0];
            }
            *write = 0;
            ++read;
        }
        else {
            size_t len = strlen(read);
            memmove(write, read, len);
            write[len] = 0;
            break;
        }
    }
    return str;
}

const char *str_escape_ex(const char *str, char *buffer, size_t size, const char *chars)
{
    size_t slen = strlen(str);
    const char *read = str;
    unsigned char *write = (unsigned char *)buffer;
    if (size < 1) {
        return NULL;
    }
    while (slen > 0 && size > 1 && *read) {
        const char *pos = strpbrk(read, chars);
        size_t len = size;
        if (pos >= read) {
            len = pos - read;
        }
        if (len < size) {
            unsigned char ch = *(const unsigned char *)pos;
            if (len > 0) {
                if (len > slen) {
                    len = slen;
                }
                memmove(write, read, len);
                slen -= len;
                write += len;
                read += len;
                size -= len;
            }
            switch (ch) {
            case '\t':
                if (size > 2) {
                    *write++ = '\\';
                    *write++ = 't';
                    size -= 2;
                }
                else size = 1;
                break;
            case '\n':
                if (size > 2) {
                    *write++ = '\\';
                    *write++ = 'n';
                    size -= 2;
                }
                else size = 1;
                break;
            case '\r':
                if (size > 2) {
                    *write++ = '\\';
                    *write++ = 'r';
                    size -= 2;
                }
                else size = 1;
                break;
            case '\"':
            case '\'':
            case '\\':
                if (size > 2) {
                    *write++ = '\\';
                    *write++ = ch;
                    size -= 2;
                }
                break;
            default:
                if (size > 5) {
                    int n = snprintf((char *)write, size, "\\%03o", ch);
                    if (n > 0) {
                        assert(n == 5);
                        write += n;
                        size -= n;
                    }
                    else size = 1;
                }
                else size = 1;
            }
            ++read;
            --slen;
        } else {
            /* end of buffer space */
            len = size - 1;
            if (len > 0) {
                if (len > slen) {
                    len = slen;
                }
                memmove(write, read, len);
                write += len;
                size -= len;
                slen -= len;
                break;
            }
        }
    }
    *write = '\0';
    return buffer;
}

const char *str_escape(const char *str, char *buffer, size_t size) {
    return str_escape_ex(str, buffer, size, "\n\t\r\'\"\\");
}
