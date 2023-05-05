#include "info.h"
#include <stdio.h>
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
    printf("\nRendszerközeli programozás\n\n");
    printf("Version: 0.1.0\n");
    printf("Fejlesztette: Svec Antal\n");
}