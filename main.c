#include <stdio.h>
#include "include/init/init.h"
#include <stdlib.h>
#include "include/info/info.h"
#include "include/data/data.h"
#include "include/bmp/bmp.h"
#include "include/pid/pid.h"
#include "include/communication/communication.h"
#include <signal.h>
#include <unistd.h>

void signal_handler(int signal)
{
    if (signal == SIGUSR1)
    {
        ReceiveViaFile(getpid());
    }
}

int main(const int argc, const char *argv[])
{
    signal(SIGINT, SignalHandler);
    signal(SIGUSR1, SignalHandler);

    Config *conf = init(argc, argv);
    print_config(conf);

    if (conf->help)
    {
        help();
    }
    else if (conf->version)
    {
        version();
    }
    else
    {
        if (conf->mode == SEND_MODE || conf->mode == DEFAULT_MODE)
        {
            if (conf->communication == FILE_COM || conf->communication == DEFAULT_COM)
            {
                printf("File communication\n");
                int *data;
                int num_values = Measurement(&data);
                SendViaFile(data, num_values);
                free(data);
            }
            else if (conf->communication == SOCKET_COM)
            {
                printf("Socket communication\n");
                int *data;
                int num_values = Measurement(&data);
                SendViaSocket(data, num_values);
                free(data);
            }
        }
        else if (conf->mode == RECEIVE_MODE)
        {
            if (conf->communication == FILE_COM || conf->communication == DEFAULT_COM)
            {
                printf("Receive mode\n");
                while (1)
                {
                    signal(SIGUSR1, signal_handler);
                    pause();
                }
            }
            else if (conf->communication == SOCKET_COM)
            {
                printf("Socket communication\n");
                ReceiveViaSocket();
            }
        }
    }

    free(conf);

    return 0;
}
