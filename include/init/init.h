#ifndef INIT_H
#define INIT_H

#include <stdbool.h>

typedef enum Option{
    NOT_AN_OPTION_O=-1,
    HELP_O,
    VERSION_O,
    SEND_O,
    RECEIVE_O,
    FILE_O,
    SOCKET_O,
}Option;

typedef enum Mode{
    DEFAULT_MODE=-1,
    SEND_MODE,
    RECEIVE_MODE,
}Mode;
typedef enum Communication{
    DEFAULT_COM=-1,
    FILE_COM,
    SOCKET_COM,
}Communication;


typedef struct Config{
    Mode mode;
    Communication communication;
}Config;

Config* init(const int, const char**);
Option get_option(const char*);
void print_config(const Config* );


#endif