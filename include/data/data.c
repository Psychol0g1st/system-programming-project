#include "data.h"
#include <stdlib.h>
#include <time.h>
#include "../helper/helper.h"

int Measurement(int **Values)
{
    srand(time(NULL));
    time_t now = time(NULL);
    struct tm *local_time = localtime(&now);
    int seconds = local_time->tm_sec;
    int minutes = local_time->tm_min;
    int values_count = (minutes * 60 + seconds) % (15 * 60);
    int num_values = max(values_count, 100);
    // int num_values = 9;
    int *values = malloc(num_values * sizeof(int));
    values[0] = 0;

    for (int i = 1; i < num_values; i++)
    {
        float rand_num = ((float)rand() / (float)(RAND_MAX));
        if (rand_num < 0.428571)
        {
            values[i] = values[i - 1] + 1;
        }
        else if (rand_num < 0.783410)
        {
            values[i] = values[i - 1] - 1;
        }
        else
        {
            values[i] = values[i - 1];
        }
    }
    *Values = values;
    return num_values;
}