#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void BMPcreator(int *Values, int NumValues)
{
    printf("BMP generalas...\n");
    int image_size = ((NumValues * 1 + 31) / 32) * 4 * NumValues;
    int row_size = image_size / NumValues;

    BitmapHeader header;
    DIBHeader dib_header;
    Palette palette;

    header.signature[0] = 'B';
    header.signature[1] = 'M';
    header.file_size = sizeof(header) + sizeof(dib_header) + sizeof(palette) + image_size;
    header.unused = 0;
    header.pixel_array_offset = sizeof(header) + sizeof(dib_header);
    dib_header.header_size = sizeof(dib_header);
    dib_header.width = NumValues;
    dib_header.height = NumValues;
    dib_header.color_planes = 1;
    dib_header.bits_per_pixel = 1;
    dib_header.compression = 0;
    dib_header.image_size = image_size;
    dib_header.x_resolution = 0;
    dib_header.y_resolution = 0;
    dib_header.colors = 2;
    dib_header.used_colors = 2;
    palette.color1 = 0x000000;
    palette.color0 = 0xFFFFFF;

    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    int f = open("chart.bmp", O_CREAT | O_TRUNC | O_WRONLY, mode);
    if (f < 0)
    {
        write(2, "File Error!", 11);
        exit(1);
    }
    unsigned char *image_data = (unsigned char *)calloc(image_size, 1);
    if (image_data == NULL)
    {
        write(2, "Memory Error!", 13);
        exit(1);
    }

    int center = NumValues / 2;
    for (int i = 0; i < NumValues; ++i)
    {
        for (int j = 0; j < NumValues; ++j)
        {
            if (center + Values[j] == i)
            {
                image_data[i * row_size + (j / 8)] |= 1 << (7 - (j % 8));
            }
        }
    }
    write(f, &header, sizeof(header));
    write(f, &dib_header, sizeof(dib_header));
    write(f, &palette, sizeof(palette));
    write(f, image_data, image_size);
    free(image_data);
    close(f);
}
