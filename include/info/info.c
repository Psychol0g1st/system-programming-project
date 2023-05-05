#include "info.h"
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

void help()
{
    printf("\nSystem programming project help:\n\n");
    printf("Usage: ./project [OPTIONS]\n\n");
    printf("Options:\n");
    printf("--help\tShow this help message.\n");
}

void version()
{
#pragma omp parallel sections
    {
#pragma omp section
        {
            printf("System programming project\n\n");
        }
#pragma omp section
        {
            printf("Version: 0.0.1\n");
        }
#pragma omp section
        {
            printf("Created by: Antal Svec\n");
        }
    }
}