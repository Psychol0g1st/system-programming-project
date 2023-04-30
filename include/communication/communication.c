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
#define BUF_SIZE 1024

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
    // create socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
    setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof on);
    // set destination address and port
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    dest_addr.sin_port = htons(3333);

    // send number of values
    int num_values = htonl(NumValues);
    ssize_t sent = sendto(sockfd, &num_values, sizeof(num_values), 0, (const struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (sent != sizeof(num_values))
    {
        perror("Error sending number of values");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // receive response
    int response;
    ssize_t received = recvfrom(sockfd, &response, sizeof(response), 0, NULL, NULL);
    if (received != sizeof(response))
    {
        perror("Error receiving response");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // check response
    if (response != NumValues)
    {
        fprintf(stderr, "Error: number of values do not match\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // send values
    ssize_t sent_values = sendto(sockfd, Values, NumValues * sizeof(int), 0, (const struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (sent_values != NumValues * sizeof(int))
    {
        perror("Error sending values");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // receive response with timeout
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_IGN; // ignore alarm signal
    if (sigaction(SIGALRM, &sa, NULL) == -1)
    {
        perror("Error setting up alarm signal handler");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    alarm(1); // set 1 second alarm
    ssize_t received_values = recvfrom(sockfd, &response, sizeof(response), 0, NULL, NULL);
    if (received_values == -1)
    {
        if (errno == EINTR) // alarm signal interrupted recvfrom
        {
            fprintf(stderr, "Error: timed out waiting for response\n");
        }
        else
        {
            perror("Error receiving response");
        }
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // check response
    if (response != NumValues * sizeof(int))
    {
        fprintf(stderr, "Error: size of values do not match\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    close(sockfd);
}

void ReceiveViaSocket()
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
    {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(3333);

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Error binding socket");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", ntohs(server_addr.sin_port));

    while (1)
    {
        char buf[BUF_SIZE];
        socklen_t client_addr_len = sizeof(client_addr);
        memset(buf, 0, BUF_SIZE);

        if (recvfrom(sock, buf, BUF_SIZE, 0, (struct sockaddr *)&client_addr, &client_addr_len) == -1)
        {
            perror("Error receiving data");
            close(sock);
            exit(EXIT_FAILURE);
        }

        // Handle first message containing int value
        int num_values = *((int *)buf);
        printf("Received num_values: %d\n", num_values);

        if (sendto(sock, &num_values, sizeof(num_values), 0, (struct sockaddr *)&client_addr, client_addr_len) == -1)
        {
            perror("Error sending data");
            close(sock);
            exit(EXIT_FAILURE);
        }

        // Handle second message containing data to be processed by BMPcreator
        int data_size = num_values * sizeof(int);
        int *data = (int *)malloc(data_size);
        memcpy(data, buf + sizeof(int), data_size);

        BMPcreator(data, num_values);

        if (sendto(sock, &data_size, sizeof(data_size), 0, (struct sockaddr *)&client_addr, client_addr_len) == -1)
        {
            perror("Error sending data");
            close(sock);
            exit(EXIT_FAILURE);
        }

        free(data);

        // receive acknowledgement with timeout
        signal(SIGALRM, NULL);
        alarm(1); // set 1 second alarm
        ssize_t received = recvfrom(sock, buf, BUF_SIZE, 0, (struct sockaddr *)&client_addr, &client_addr_len);
        if (received == -1 && errno == EINTR)
        {
            fprintf(stderr, "Error: timeout occurred while waiting for acknowledgement\n");
            continue; // continue waiting for next message
        }
        else if (received == -1)
        {
            perror("Error receiving acknowledgement");
            close(sock);
            exit(EXIT_FAILURE);
        }
        else if (received != sizeof(int))
        {
            fprintf(stderr, "Error: invalid acknowledgement size received\n");
            continue; // continue waiting for next message
        }

        int ack;
        memcpy(&ack, buf, sizeof(int));

        if (ack != data_size)
        {
            fprintf(stderr, "Error: size of data processed do not match\n");
            continue; // continue waiting for next message
        }

        printf("Data processed successfully. Acknowledgement sent.\n");
    }

    close(sock);
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