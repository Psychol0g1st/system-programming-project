#include "communication.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "../pid/pid.h"
#include "../bmp/bmp.h"
#include "../helper/helper.h"
#define MAX_VALUES 1000
#define BUFSIZE 1024

void SendViaFile(int *Values, int NumValues)
{
    printf("Kuldes fajlba...\n");
    struct passwd *pw = getpwuid(getuid());                  // get user's information
    const char *homedir = pw->pw_dir;                        // get user's home directory
    char file_path[256];                                     // create a buffer for file path
    sprintf(file_path, "%s/%s", homedir, "Measurement.txt"); // build file path
    FILE *file = fopen(file_path, "w");                      // open file for writing
    if (file == NULL)
    {
        error_with_exit(3, "Hiba! Nem sikerult megnyitni a Measurement.txt fajlt!\n");
    }

    for (int i = 0; i < NumValues; i++)
    {
        fprintf(file, "%d\n", Values[i]);
    }

    fclose(file);

    int pid = FindPID();
    if (pid == -1)
    {
        error_with_exit(11, "Hiba! Nem talalhato a chart process!");
    }
    else
    {
        kill(pid, SIGUSR1);
    }
}

void ReceiveViaFile()
{
    printf("Fogadas fajlbol...\n");
    struct passwd *pw = getpwuid(getuid());                  // get user's information
    const char *homedir = pw->pw_dir;                        // get user's home directory
    char file_path[256];                                     // create a buffer for file path
    sprintf(file_path, "%s/%s", homedir, "Measurement.txt"); // build file path
    FILE *file = fopen(file_path, "r");                      // open file for writing
    if (file == NULL)
    {
        error_with_exit(3, "Hiba! Nem sikerult megnyitni a Measurement.txt fajlt!\n");
    }

    int *values = malloc(MAX_VALUES * sizeof(int));
    int num_values = 0;
    int value;
    while (fscanf(file, "%d", &value) == 1)
    {
        values[num_values] = value;
        num_values++;
    }

    fclose(file);
    BMPcreator(values, num_values);
    free(values);
}

void SendViaSocket(int *Values, int NumValues)
{
    printf("Kuldes socketen...\n");
    int s;                     // socket ID
    int bytes;                 // received/sent bytes
    int flag;                  // transmission flag
    char on;                   // sockopt option
    unsigned int server_size;  // length of the sockaddr_in server
    struct sockaddr_in server; // address of server

    /************************ Initialization ********************/
    on = 1;
    flag = 0;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(3333);
    server_size = sizeof server;

    /************************ Creating socket *******************/
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {
        error_with_exit(5, "Hiba! Nem sikerult letrehozni a socketet!\n");
    }
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
    setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof on);

    /************************ Sending data **********************/
    bytes = sendto(s, &NumValues, sizeof(NumValues), flag, (struct sockaddr *)&server, server_size);
    if (bytes <= 0)
    {
        error_with_exit(6, "Hiba! Nem sikerult elkuldeni a meresi eredmenyt socketen!\n");
    }

    signal(SIGALRM, SignalHandler);
    alarm(1);

    int checkNumValues = 0;
    /************************ Receive data **********************/
    bytes = recvfrom(s, &checkNumValues, sizeof(int), flag, (struct sockaddr *)&server, &server_size);
    if (bytes < 0)
    {
        error_with_exit(7, "Hiba! Nem sikerult fogadni a meresi eredmenyt socketen!\n");
    }
    alarm(0);
    if (checkNumValues != NumValues)
    {
        error_with_exit(8, "Hiba! A fogadott meresi eredmeny nem egyezik a kuldottel!\n");
    }
    bytes = sendto(s, Values, sizeof(int) * NumValues, flag, (struct sockaddr *)&server, server_size);
    if (bytes <= 0)
    {
        error_with_exit(6, "Hiba! Nem sikerult elkuldeni a meresi eredmenyt socketen!\n");
    }
    bytes = recvfrom(s, &checkNumValues, sizeof(int), flag, (struct sockaddr *)&server, &server_size);
    if (bytes < 0)
    {
        error_with_exit(7, "Hiba! Nem sikerult fogadni a meresi eredmenyt socketen!\n");
    }
    if (checkNumValues != NumValues)
    {
        error_with_exit(8, "Hiba! A fogadott meresi eredmeny nem egyezik a kuldottel!\n");
    }
}

void ReceiveViaSocket()
{
    printf("Fogadas socketen...\n");
    int s;                     // socket descriptor
    int bytes;                 // received/sent bytes
    int err;                   // error code
    int flag;                  // transmission flag
    char on;                   // sockopt option
    int NumValues;             // number of values
    unsigned int server_size;  // length of the sockaddr_in server
    unsigned int client_size;  // length of the sockaddr_in client
    struct sockaddr_in server; // address of server
    struct sockaddr_in client; // address of client

    /************************ Initialization ********************/
    on = 1;
    flag = 0;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(3333);
    server_size = sizeof server;
    client_size = sizeof client;

    /************************ Creating socket *******************/
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {
        error_with_exit(5, "Hiba! Nem sikerult letrehozni a socketet!\n");
    }
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
    setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof on);

    /************************ Binding socket ********************/
    err = bind(s, (struct sockaddr *)&server, server_size);
    if (err < 0)
    {
        error_with_exit(9, "Hiba! Nem sikerult a socketet hozzarendelni a cimhez!\n");
    }

    while (1)
    {
        /************************ Receive data **********************/
        printf("\n Varakozas...\n");
        bytes = recvfrom(s, &NumValues, sizeof(NumValues), flag, (struct sockaddr *)&client, &client_size);
        if (bytes < 0)
        {
            error_with_exit(7, "Hiba! Nem sikerult fogadni a meresi eredmenyt socketen!\n");
        }

        /************************ Sending data **********************/
        bytes = sendto(s, &NumValues, sizeof(NumValues), flag, (struct sockaddr *)&client, client_size);
        if (bytes <= 0)
        {
            error_with_exit(6, "Hiba! Nem sikerult elkuldeni a meresi eredmenyt socketen!\n");
        }

        int data_size = NumValues * sizeof(int);
        int *data = (int *)malloc(data_size);

        bytes = recvfrom(s, data, sizeof(int) * NumValues, flag, (struct sockaddr *)&client, &client_size);
        if (bytes < 0)
        {
            error_with_exit(7, "Hiba! Nem sikerult fogadni a meresi eredmenyt socketen!\n");
        }

        int receivedNumValues = bytes / sizeof(int);
        /************************ Sending data **********************/
        bytes = sendto(s, &receivedNumValues, sizeof(int), flag, (struct sockaddr *)&client, client_size);
        if (bytes <= 0)
        {
            error_with_exit(6, "Hiba! Nem sikerult elkuldeni a meresi eredmenyt socketen!\n");
        }
        if (receivedNumValues != NumValues)
        {
            error_with_exit(8, "Hiba! A fogadott meresi eredmeny nem egyezik a kuldottel!\n");
        }
        BMPcreator(data, NumValues);
        free(data);
    }
}

void SignalHandler(int sig)
{
    switch (sig)
    {
    case SIGINT:
        printf("Kilepes a programbol...\n");
        exit(0);
    case SIGUSR1:
        printf("Hiba! Fajl kuldes nem lehetseges!\n");
        break;
    case SIGALRM:
        printf("Hiba: Szerver nem elerheto!\n");
        exit(1);
    default:
        printf("Varatlan jel erkezett: %d\n", sig);
    }
}