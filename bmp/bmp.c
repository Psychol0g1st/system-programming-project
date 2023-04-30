#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 8
#pragma pack(push, 1)
typedef struct BitmapHeader
{
    unsigned char signature[2];
    unsigned int file_size;
    unsigned int unused;
    unsigned int pixel_array_offset;
} BitmapHeader;
#pragma pack(pop)
#pragma pack(push, 1)
typedef struct DIBHeader
{
    unsigned int header_size;
    int width;
    int height;
    unsigned short color_planes;
    unsigned short bits_per_pixel;
    unsigned int compression;
    unsigned int image_size;
    int x_resolution;
    int y_resolution;
    unsigned int colors;
    unsigned int used_colors;
} DIBHeader;
#pragma pack(pop)
typedef struct Palette
{
    unsigned int color0;
    unsigned int color1;
} Palette;

void generate_bmp(char *bmp, const unsigned int file_size, const int *values)
{

    int padding_size = (4 - (SIZE / 8) % 4) % 4;
    int row_size = (SIZE * 1 + 7) / 8 + padding_size;
    int image_size = row_size * SIZE;

    BitmapHeader header;
    DIBHeader dib_header;
    Palette palette;
    header.signature[0] = 'B';
    header.signature[1] = 'M';
    header.file_size = sizeof(header) + sizeof(dib_header) + sizeof(palette) + image_size;
    header.unused = 0;
    header.pixel_array_offset = sizeof(header) + sizeof(dib_header);
    dib_header.header_size = sizeof(dib_header);
    dib_header.width = SIZE;
    dib_header.height = SIZE;
    dib_header.color_planes = 1;
    dib_header.bits_per_pixel = 1;
    dib_header.compression = 0;
    dib_header.image_size = image_size; // 1 - bit depth, 31 - padding
    dib_header.x_resolution = 0;
    dib_header.y_resolution = 0;
    dib_header.colors = 2;
    dib_header.used_colors = 2;
    palette.color1 = 0x000000;
    palette.color0 = 0xFFFFFF;

    int f = open("image.bmp", O_CREAT | O_TRUNC | O_WRONLY, S_IWUSR | S_IRUSR);
    if (f < 0)
    {
        write(2, "File Error!", 11);
        exit(1);
    }

    unsigned char image[8][1] = {
        {0xAA}, // row 1: 0 1 0 1 0 1 0 1
        {0x55}, // row 0: 1 0 1 0 1 0 1 0
        {0xAA}, // row 3: 0 1 0 1 0 1 0 1
        {0x55}, // row 2: 1 0 1 0 1 0 1 0
        {0xAA}, // row 5: 0 1 0 1 0 1 0 1
        {0x55}, // row 4: 1 0 1 0 1 0 1 0
        {0xAA}, // row 7: 0 1 0 1 0 1 0 1
        {0x55}  // row 6: 1 0 1 0 1 0 1 0
    };

    // Allocate memory for image data
    unsigned char *image_data = (unsigned char *)calloc(image_size, 1);

    // Copy image data to image_data buffer, including padding
    int index = 0;
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < row_size; j++)
        {
            if (j < (SIZE / 8))
            {
                image_data[index++] = image[i][j];
            }
            else
            {
                image_data[index++] = 0;
            }
        }
    }
    write(f, &header, sizeof(header));
    write(f, &dib_header, sizeof(dib_header));
    write(f, &palette, sizeof(palette));
    write(f, image_data, image_size);
    printf("File size: %d\n", header.file_size);
    free(image_data);
    close(f);
}

int main()
{
    int *values = (int *)malloc(SIZE * sizeof(int));
    for (int i = 0; i < SIZE; ++i)
    {
        values[i] = random() % SIZE;
        printf("%d ", values[i]);
    }
    puts("");
    int file_size = sizeof(BitmapHeader) + sizeof(DIBHeader) + SIZE * SIZE / 8;
    char *bmp = (char *)calloc(file_size, 1);
    generate_bmp(bmp, file_size, values);

    free(values);

    return 0;
}