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
#define MAX_VALUES 1000
#define BUFSIZE 1024

void SendViaFile(int *Values, int NumValues)
{

    struct passwd *pw = getpwuid(getuid());                  // get user's information
    const char *homedir = pw->pw_dir;                        // get user's home directory
    char file_path[256];                                     // create a buffer for file path
    sprintf(file_path, "%s/%s", homedir, "Measurement.txt"); // build file path
    FILE *file = fopen(file_path, "w");                      // open file for writing
    if (file == NULL)
    {
        perror("error creating file");
        exit(EXIT_FAILURE);
    }

    // write values to file
    for (int i = 0; i < NumValues; i++)
    {
        fprintf(file, "%d\n", Values[i]);
    }

    // close file
    fclose(file);

    // find PID and send signal
    int pid = FindPID();
    if (pid == -1)
    {
        printf("error: no receiver process found\n");
        // exit(EXIT_FAILURE);
    }
    else
    {
        kill(pid, SIGUSR1);
    }
}

void ReceiveViaFile(int sig)
{
    struct passwd *pw = getpwuid(getuid());                  // get user's information
    const char *homedir = pw->pw_dir;                        // get user's home directory
    char file_path[256];                                     // create a buffer for file path
    sprintf(file_path, "%s/%s", homedir, "Measurement.txt"); // build file path
    FILE *file = fopen(file_path, "r");                      // open file for writing
    if (file == NULL)
    {
        perror("error creating file");
        exit(EXIT_FAILURE);
    }

    // Memóriaterületet foglalunk a beolvasott értékeknek
    int *values = malloc(MAX_VALUES * sizeof(int));
    int num_values = 0;

    // Beolvassuk a fájl tartalmát és eltároljuk az értékeket
    int value;
    while (fscanf(file, "%d", &value) == 1)
    {
        values[num_values] = value;
        num_values++;
        if (num_values == MAX_VALUES)
        {
            printf("Tul sok ertek van a fajlban!\n");
            exit(1);
        }
    }

    // Felszabadítjuk a fájl erőforrásait
    fclose(file);

    // BMPcreator eljárás meghívása a beolvasott értékekkel
    BMPcreator(values, num_values);

    // Felszabadítjuk a memóriaterületet
    free(values);
}

void SendViaSocket(int *Values, int NumValues)
{
    int s;                     // socket ID
    int bytes;                 // received/sent bytes
    int flag;                  // transmission flag
    char on;                   // sockopt option
    char buffer[BUFSIZE];      // datagram buffer area
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
        fprintf(stderr, "Socket creation error.\n");
        exit(2);
    }
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
    setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof on);

    /************************ Sending data **********************/
    printf(" Message to send: %d", NumValues);
    bytes = sendto(s, &NumValues, sizeof(NumValues), flag, (struct sockaddr *)&server, server_size);
    if (bytes <= 0)
    {
        fprintf(stderr, "  Sending error.\n");
        exit(3);
    }
    printf(" %i bytes have been sent to server.\n", bytes);
    signal(SIGALRM, SignalHandler);

    alarm(1);
    int checkNumValues = 0;
    /************************ Receive data **********************/
    bytes = recvfrom(s, &checkNumValues, sizeof(int), flag, (struct sockaddr *)&server, &server_size);
    if (bytes < 0)
    {
        fprintf(stderr, " Receiving error.\n");
        exit(4);
    }
    alarm(0);
    printf(" Server's (%s:%d) acknowledgement:\n  %d\n",
           inet_ntoa(server.sin_addr), ntohs(server.sin_port), checkNumValues);
    if (checkNumValues != NumValues)
    {

        printf("  The number of values are not equal.\n");
        exit(5);
    }
    printf(" Array size to send: %d", NumValues);
    bytes = sendto(s, Values, sizeof(int) * NumValues, flag, (struct sockaddr *)&server, server_size);
    if (bytes <= 0)
    {
        fprintf(stderr, "  Sending array error.\n");
        exit(3);
    }
    printf(" %i bytes have been sent to server.\n", bytes);

    bytes = recvfrom(s, &checkNumValues, sizeof(int), flag, (struct sockaddr *)&server, &server_size);
    if (bytes < 0)
    {
        fprintf(stderr, " Receiving error.\n");
        exit(4);
    }
    printf("Got %d values from server\n", checkNumValues);
    if (checkNumValues != NumValues)
    {

        printf("  The number of values are not equal.\n");
        exit(5);
    }
    /************************ Closing ***************************/
    return EXIT_SUCCESS;
}

void ReceiveViaSocket(int sig)
{
    int s;                     // socket descriptor
    int bytes;                 // received/sent bytes
    int err;                   // error code
    int flag;                  // transmission flag
    char on;                   // sockopt option
    char buffer[BUFSIZE];      // datagram buffer area
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
        fprintf(stderr, " Socket creation error.\n");
        exit(2);
    }
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
    setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof on);

    /************************ Binding socket ********************/
    err = bind(s, (struct sockaddr *)&server, server_size);
    if (err < 0)
    {
        fprintf(stderr, "Binding error.\n");
        exit(3);
    }

    while (1)
    { // Continuous server operation
        /************************ Receive data **********************/
        printf("\n Waiting for a message...\n");
        bytes = recvfrom(s, &NumValues, sizeof(NumValues), flag, (struct sockaddr *)&client, &client_size);
        if (bytes < 0)
        {
            fprintf(stderr, " Receiving error.\n");
            exit(4);
        }
        printf(" %d bytes have been received from the client (%s:%d).\n Client's message:  %d\n",
               bytes, inet_ntoa(client.sin_addr), ntohs(client.sin_port), NumValues);

        /************************ Sending data **********************/
        bytes = sendto(s, &NumValues, sizeof(NumValues), flag, (struct sockaddr *)&client, client_size);
        if (bytes <= 0)
        {
            fprintf(stderr, " Sending error.\n");
            exit(5);
        }
        printf(" Acknowledgement have been sent to client.\n");
        int data_size = NumValues * sizeof(int);
        int *data = (int *)malloc(data_size);
        printf("\n Waiting for array of data...\n");
        bytes = recvfrom(s, data, sizeof(int) * NumValues, flag, (struct sockaddr *)&client, &client_size);
        if (bytes < 0)
        {
            fprintf(stderr, " Receiving array error.\n");
            exit(4);
        }
        int receivedNumValues = bytes / sizeof(int);
        printf(" %d bytes have been received from the client (%s:%d).\n Array got size:\n  %d",
               bytes, inet_ntoa(client.sin_addr), ntohs(client.sin_port), receivedNumValues);

        /************************ Sending data **********************/
        // sprintf(buffer, "I have received a %d bytes long message.", bytes - 1);
        bytes = sendto(s, &receivedNumValues, sizeof(int), flag, (struct sockaddr *)&client, client_size);
        if (bytes <= 0)
        {
            fprintf(stderr, " Sending error.\n");
            exit(5);
        }
        if (receivedNumValues != NumValues)
        {
            printf("  The number of values are not equal.\n");
            exit(5);
        }
        printf(" Acknowledgement about gotten array size have been sent to client.\n");
        BMPcreator(data, NumValues);
        free(data);
    }
}

void SignalHandler(int sig)
{
    switch (sig)
    {
    case SIGINT:
        printf("Exiting program...\n");
        exit(0);
    case SIGUSR1:
        printf("Error: File transfer service not available!\n");
        break;
    case SIGALRM:
        printf("Error: Server did not respond within timeout!\n");
        exit(1);
    default:
        printf("Received unexpected signal: %d\n", sig);
    }
}