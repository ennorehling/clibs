#pragma once
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

#ifndef _L10N_H
#define _L10N_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#define L10N_PREFIX_STRING '$'
#define L10N_PREFIX_INT '%'
#define L10N_PREFIX_L10N '@'
#define L10N_PREFIX_EVAL '!'

#define L10N_TOKEN_MAXLEN 16

    typedef struct l10n_arg {
        char *name;
        union {
            char *s;
            void *v;
            int i;
        } value;
    } l10n_arg;

    typedef struct l10n_text {
        const char *format;
        struct l10n_arg *args;
        int refcount;
    } l10n_text;

    l10n_text *l10n_text_build(const char *format, ...);
    l10n_text *l10n_text_create(void);
    void l10n_text_assign(l10n_text *txt, const char *format, ...);
    l10n_text *l10n_text_copy(l10n_text *txt);
    l10n_text *l10n_text_release(l10n_text *txt);
    char *l10n_text_render(l10n_text *txt, char *buffer, size_t len);

#ifdef __cplusplus
}
#endif
#endif
