#pragma once

#define CMD_TOKENS_MAX   256
#define CMD_SWITCHES_MAX 256
#define CMD_PARAMS_MAX   256

typedef struct cmd_param {
    char *key;
    char *value;
} cmd_param;

void parse_command_line(int argc, char **argv,
        char * tokens[], char * switches[],
        cmd_param params[]);

#ifdef CMDLINE_IMPLEMENTATION
#include <string.h>
void parse_command_line(int argc, char **argv,
        char * tokens[], char * switches[],
        cmd_param params[])
{
    int ntoken = 0;
    int nparam = 0;
    int nswitch = 0;
    for (int i = 0; i != argc; ++i)
    {
        char *arg = argv[i];
        if (arg[0]=='-' || arg[0] =='/') {
            char * s = strchr(++arg, '=');
            if (s > arg) {
                if (nparam + 1 < CMD_PARAMS_MAX) {
                    cmd_param *param = params + nparam++;
                    *s++ = 0;
                    param->key = arg;
                    param->value = s;
                }
            }
            else if (nswitch + 1 < CMD_SWITCHES_MAX) {
                switches[nswitch++] = arg;
            }
        }
        else if (ntoken + 1 < CMD_TOKENS_MAX) {
            tokens[ntoken++] = arg;
        }
    }
    params[nparam].key = 0;
    tokens[ntoken] = 0;
    switches[nswitch] = 0;
}

#endif
