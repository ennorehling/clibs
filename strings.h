#pragma once
#include <stddef.h>
#include <string.h>

void str_replace(char *buffer, size_t size, const char *tmpl, const char *var, const char *value);
int str_hash(const char *s);
const char *str_itoa(int i);
size_t str_slprintf(char * dst, size_t size, const char * format, ...);
size_t str_strlcpy(char *dst, const char *src, size_t len);
size_t str_strlcat(char *dst, const char *src, size_t len);
char *str_strdup(const char *s);

const char *str_escape(const char *str, char *buffer, size_t size);
const char *str_escape_ex(const char *str, char *buffer, size_t size, const char *chars);
char *str_unescape(char *str);

unsigned int jenkins_hash(unsigned int a);
unsigned int wang_hash(unsigned int a);

/* static buffered string */
typedef struct sbstring {
    size_t size;
    char *begin;
    char *end;
} sbstring;

void sbs_printf(struct sbstring *sbs, const char *format, ...);
void sbs_init(struct sbstring *sbs, char *buffer, size_t size);
void sbs_adopt(struct sbstring *sbs, char *buffer, size_t size);
void sbs_strcat(struct sbstring *sbs, const char *str);
void sbs_strncat(struct sbstring *sbs, const char *str, size_t size);
void sbs_substr(struct sbstring *sbp, ptrdiff_t pos, size_t len);
size_t sbs_length(const struct sbstring *sbs);

/* benchmark for units:
    * JENKINS_HASH: 5.25 misses/hit (with good cache behavior)
    * WANG_HASH:    5.33 misses/hit (with good cache behavior)
    * KNUTH_HASH:   1.93 misses/hit (with bad cache behavior)
    * CF_HASH:      fucking awful!
    */
#define KNUTH_HASH1(a, m) ((a) % m)
#define KNUTH_HASH2(a, m) (m - 2 - a % (m-2))
#define CF_HASH1(a, m) ((a) % m)
#define CF_HASH2(a, m) (8 - ((a) & 7))
#define JENKINS_HASH1(a, m) (jenkins_hash(a) % m)
#define JENKINS_HASH2(a, m) 1
#define WANG_HASH1(a, m) (wang_hash(a) % m)
#define WANG_HASH2(a, m) 1

#define HASH1 JENKINS_HASH1
#define HASH2 JENKINS_HASH2
#define slprintf str_slprintf
