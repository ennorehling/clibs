/*
Copyright (c) 2012, Enno Rehling <enno@eressea.de>

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
**/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "l10n.h"

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef _MSC_VER
#define strdup(s) _strdup(s)
#define snprintf _snprintf
#endif

#define NUM_ARGS 8

static int read_token(const char *buffer, char *token, size_t len)
{
    char *po = token;
    const char *pi = buffer;
    if (*pi == '{') ++pi;
    while (*pi && po < token + len) {
        if (*pi == '}') {
            ++pi;
            break;
        }
        if (!isalnum(*pi)) {
            break;
        }
        *po++ = *pi++;
    }
    *po = '\0';
    return (int)(pi - buffer);
}

static l10n_arg *get_arg(l10n_arg *args, const char *token)
{
    int i;
    for (i = 0; i != NUM_ARGS && args[i].name; ++i) {
        if (strcmp(args[i].name + 1, token) == 0) {
            return args + i;
        }
    }
    return NULL;
}

void l10n_text_init(l10n_text *txt)
{
    txt->format = NULL;
    txt->args = NULL;
    txt->refcount = 0;
}

static void free_args(l10n_arg args[])
{
    int i;
    for (i = 0; i != NUM_ARGS && args[i].name; ++i) {
        char type = args[i].name[0];
        switch (type) {
        case L10N_PREFIX_L10N:
            l10n_text_release((l10n_text *)args[i].value.v);
            break;
        case L10N_PREFIX_STRING:
            free(args[i].value.s);
            break;
        default:
            break;
        }
        free(args[i].name);
    }
    free(args);
}

void l10n_text_free(l10n_text *txt)
{
    assert(txt->refcount == 0);
    txt->format = NULL;
    if (txt->args) {
        free_args(txt->args);
        txt->args = NULL;
    }
}

l10n_text *l10n_text_create(void)
{
    l10n_text *txt = calloc(1, sizeof(l10n_text));
    if (txt) {
        l10n_text_init(txt);
        txt->refcount = 1;
    }
    return txt;
}

l10n_text *l10n_text_copy(l10n_text *txt)
{
    assert(txt->refcount>0);
    ++txt->refcount;
    return txt;
}

l10n_text *l10n_text_release(l10n_text *txt)
{
    if (--txt->refcount == 0) {
        l10n_text_free(txt);
        free(txt);
        txt = NULL;
    }
    return txt;
}

char *l10n_text_render(l10n_text *txt, char *buffer, size_t len)
{
    char * po = buffer;

    if (txt->format) {
        const char *pi = txt->format;
        while (*pi && po < buffer + len) {
            char token[L10N_TOKEN_MAXLEN];
            size_t count = len - (po - buffer) - 1;
            if (*pi == L10N_PREFIX_STRING) {
                l10n_arg * arg;
                int c;
                c = read_token(++pi, token, L10N_TOKEN_MAXLEN);
                pi += c;
                arg = get_arg(txt->args, token);
                // TODO: use strncpy instead?
                c = snprintf(po, count, "%s", arg->value.s);
                po += c;
            }
            else if (*pi == L10N_PREFIX_INT) {
                l10n_arg * arg;
                int c;

                c = read_token(++pi, token, L10N_TOKEN_MAXLEN);
                pi += c;
                arg = get_arg(txt->args, token);
                // TODO: use itoa instead?
                c = snprintf(po, count, "%d", arg->value.i);
                po += c;
            }
            else if (*pi == L10N_PREFIX_L10N) {
                l10n_text *sub;
                l10n_arg * arg;
                int c;
                c = read_token(++pi, token, L10N_TOKEN_MAXLEN);
                pi += c;
                arg = get_arg(txt->args, token);
                sub = (l10n_text *)arg->value.v;
                l10n_text_render(sub, po, count);
                po += strlen(po);
            }
            else {
                *po++ = *pi++;
            }
        }
    }
    *po = '\0';
    return buffer;
}

static l10n_text *l10n_text_assign_va(l10n_text *txt, const char * format, va_list va)
{
    l10n_arg args[NUM_ARGS];
    int i;

    for (i = 0; i != NUM_ARGS; ++i) {
        const char *name = va_arg(va, const char *);
        if (!name) {
            args[i].name = NULL;
            break;
        }
        if (name[0] == L10N_PREFIX_STRING) {
            const char *str = va_arg(va, const char *);
            args[i].value.s = strdup(str);
        }
        else if (name[0] == L10N_PREFIX_INT) {
            args[i].value.i = va_arg(va, int);
        }
        else if (name[0] == L10N_PREFIX_L10N) {
            l10n_text *a = va_arg(va, l10n_text *);
            args[i].value.v = l10n_text_copy(a);
        }
        else {
            assert(!"invalid token");
        }
        args[i].name = strdup(name);
    }
    txt->format = format;
    txt->args = malloc((i+1) * sizeof(l10n_arg));
    memcpy(txt->args, args, i * sizeof(l10n_arg));
    txt->args[i].name = NULL;
    return txt;
}

void l10n_text_assign(l10n_text *txt, const char *format, ...)
{
    va_list va;
    if (txt->args) {
        /* TODO: combine code with assign_va to realloc these? */
        free_args(txt->args);
        txt->args = NULL;
        txt->format = NULL;
    }
    if (format) {
        va_start(va, format);
        l10n_text_assign_va(txt, format, va);
        va_end(va);
    }
}

l10n_text *l10n_text_build(const char *format, ...)
{
    l10n_text *txt = l10n_text_create();
    if (txt) {
        va_list va;
        va_start(va, format);
        l10n_text_assign_va(txt, format, va);
        va_end(va);
    }
    return txt;
}
