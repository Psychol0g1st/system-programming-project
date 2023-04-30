#ifndef BMP_H
#define BMP_H


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

void BMPcreator(int*, int);
#endif