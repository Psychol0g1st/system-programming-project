#include "helper.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

void error_with_exit(const int code, const char *message)
{
    puts(message);
    exit(code);
}

void str_to_lower(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = tolower(str[i]);
    }
}
int max(int a, int b)
{
    return a > b ? a : b;
}