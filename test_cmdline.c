#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif
#define CMDLINE_IMPLEMENTATION
#include "cmdline.h"

#include "CuTest.h"
#include <string.h>

static void test_parse_cmdline(CuTest *tc)
{
    char *tokens[CMD_TOKENS_MAX];
    char *switches[CMD_SWITCHES_MAX];
    cmd_param params[CMD_PARAMS_MAX];
    parse_command_line(0, NULL, tokens, switches, params);
    CuAssertPtrEquals(tc, NULL, tokens[0]);
    CuAssertPtrEquals(tc, NULL, switches[0]);
    CuAssertPtrEquals(tc, NULL, params[0].key);

    const char *input = "-test -foo=bar one two -help three";
    char copy[64];
    char *argv[6];
    int argc = 0;
    memcpy(copy, input, strlen(input)+1);
    char *tok = strtok(copy, " ");
    while (tok) {
        argv[argc++] = tok;
        tok = strtok(NULL, " ");
    }
    parse_command_line(argc, argv, tokens, switches, params);
    CuAssertStrEquals(tc, "one", tokens[0]);
    CuAssertStrEquals(tc, "two", tokens[1]);
    CuAssertStrEquals(tc, "three", tokens[2]);
    CuAssertPtrEquals(tc, NULL, tokens[3]);
    CuAssertStrEquals(tc, "test", switches[0]);
    CuAssertStrEquals(tc, "help", switches[1]);
    CuAssertPtrEquals(tc, NULL, switches[2]);
    CuAssertStrEquals(tc, "foo", params[0].key);
    CuAssertStrEquals(tc, "bar", params[0].value);
    CuAssertPtrEquals(tc, NULL, params[1].key);
}

void add_suite_cmdline(CuSuite *suite)
{
      SUITE_ADD_TEST(suite, test_parse_cmdline);
}

