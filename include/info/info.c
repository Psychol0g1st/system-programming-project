#include "info.h"
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

void help()
{
    printf("\nRendszerközeli programozás projekt súgó:\n\n");
    printf("Futtatás: ./chart [PARAMÉTEREK]\n\n");
    printf("Paraméterek:\n");
    printf("--help\tEzt a sugót megjeleníti.\n");
    printf("-send\t Küldési mód. Alapértelmezetten fájl segítségével.");
    printf("-send -file\t Küldési mód fájlon keresztül.");
    printf("-send -socket\t Küldési mód socketen keresztül.");
    printf("-receive\t Fogadási mód. Alapértelmezetten fájlon keresztül.");
    printf("-receive -file\t Fogadási mód fájlon keresztül.");
    printf("-receive -socket\t Fogadási mód socketen keresztül.");
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
            printf("Version: 0.1.0\n2023.05.01\n\n");
        }
#pragma omp section
        {
            printf("Created by: Antal Svec\n");
        }
    }
}