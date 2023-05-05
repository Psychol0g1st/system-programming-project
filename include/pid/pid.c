#include "pid.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>
#include "../helper/helper.h"

int FindPID()
{
    printf("PID keresése...\n");
    DIR *proc_dir = opendir("/proc");
    if (proc_dir == NULL)
    {
        error_with_exit(10, "Hiba! Nem sikerült megnyítni a /proc könyvtárat!\n");
    }

    struct dirent *entry;
    char path[512];
    int chart_pid = -1;
    int pid = getpid();
    while ((entry = readdir(proc_dir)) != NULL)
    {
        if (entry->d_type == DT_DIR && isdigit(entry->d_name[0]))
        {
            snprintf(path, sizeof(path), "/proc/%s/status", entry->d_name);

            FILE *status_file = fopen(path, "r");
            if (status_file == NULL)
            {
                continue;
            }

            char line[256];
            if (fgets(line, sizeof(line), status_file) == NULL)
            {
                fclose(status_file);
                continue;
            }
            if (strstr(line, "chart") != NULL)
            {
                while (fgets(line, sizeof(line), status_file) != NULL)
                {
                    if (sscanf(line, "Pid:\t%d", &chart_pid) == 1)
                    {
                        if (chart_pid != pid)
                        {
                            break;
                        }
                        else
                        {
                            chart_pid = -1;
                        }
                    }
                }
            }

            fclose(status_file);
            if (chart_pid != -1)
            {
                break;
            }
        }
    }

    closedir(proc_dir);
    return chart_pid;
}
