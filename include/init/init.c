#include "init.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../helper/helper.h"

#define FILE_NAME "chart"

Config *init(const int argc, const char *argv[])
{
    const int len1 = strlen(argv[0]);
    const int len2 = strlen(FILE_NAME);

    if (strcmp(argv[0] + len1 - len2, FILE_NAME) != 0)
    {
        error_with_exit(1, "Hiba! A futtatott program neve nem 'chart'!");
    }
    Config *conf = (Config *)malloc(sizeof(Config));

    conf->mode = DEFAULT_MODE;
    conf->communication = DEFAULT_COM;

    for (int i = 1; i < argc; ++i)
    {
        Option opt = get_option(argv[i]);
        switch (opt)
        {
        case HELP_O:
            conf->help = true;
            break;
        case VERSION_O:
            conf->version = true;
            break;
        case SEND_O:
            if (conf->mode != DEFAULT_MODE)
            {
                conf->help = true;
            }
            else
            {
                conf->mode = SEND_MODE;
            }
            break;
        case RECEIVE_O:
            if (conf->mode != DEFAULT_MODE)
            {
                conf->help = true;
            }
            else
            {
                conf->mode = RECEIVE_MODE;
            }
            break;
        case FILE_O:
            if (conf->communication != DEFAULT_COM)
            {
                conf->help = true;
            }
            else
            {
                conf->communication = FILE_COM;
            }
            break;
        case SOCKET_O:
            if (conf->communication != DEFAULT_COM)
            {
                conf->help = true;
            }
            else
            {
                conf->communication = SOCKET_COM;
            }
            break;
        default:
            conf->help = true;
            break;
        }
    }
    return conf;
}

Option get_option(const char *str)
{
    char *s = (char *)malloc(strlen(str));
    strcpy(s, str);
    str_to_lower(s);
    Option opt;
    if (strcmp(s, "--help") == 0)
    {
        opt = HELP_O;
    }
    else if (strcmp(s, "--version") == 0)
    {
        opt = VERSION_O;
    }
    else if (strcmp(s, "-send") == 0)
    {
        opt = SEND_O;
    }
    else if (strcmp(s, "-receive") == 0)
    {
        opt = RECEIVE_O;
    }
    else if (strcmp(s, "-file") == 0)
    {
        opt = FILE_O;
    }
    else if (strcmp(s, "-socket") == 0)
    {
        opt = SOCKET_O;
    }
    else
    {
        opt = NOT_AN_OPTION_O;
    }
    free(s);
    return opt;
}

void print_config(const Config *conf)
{
    printf("%d\n", conf->mode);
    printf("%d\n", conf->communication);
}