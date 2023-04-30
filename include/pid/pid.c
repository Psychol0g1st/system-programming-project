#include "pid.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>

int FindPID()
{
    DIR *proc_dir = opendir("/proc");
    if (proc_dir == NULL)
    {
        perror("Cannot open /proc directory");
        return -1;
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
                perror(path);
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
                    printf("%s\n", line);
                    if (sscanf(line, "Pid:\t%d", &chart_pid) == 1)
                    {

                        printf("%d\n", chart_pid);
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
