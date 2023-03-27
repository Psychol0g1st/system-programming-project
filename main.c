#include <stdio.h>
#include "include/init/init.h"
#include <stdlib.h>

int main(const int argc, const char* argv[]){

    Config* conf = init(argc, argv);
    print_config(conf);
    free(conf);

    return 0;
}