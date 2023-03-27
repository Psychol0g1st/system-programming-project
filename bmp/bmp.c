#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 8
#pragma pack(push, 1)
typedef struct BitmapHeader{
    unsigned char signature[2];
    unsigned int file_size;
    unsigned int unused;
    unsigned int pixel_array_offset;
}BitmapHeader;
#pragma pack(pop)
#pragma pack(push, 1)
typedef struct DIBHeader{
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
}DIBHeader;
#pragma pack(pop)
typedef struct Palette{
    unsigned int color0;
    unsigned int color1;
}Palette;


void generate_bmp(char* bmp, const unsigned int file_size, const int* values){
    BitmapHeader header;
    DIBHeader dib_header;
    Palette palette;
    header.signature[0] = 'B';
    header.signature[1] = 'M';
    header.file_size = sizeof(header) + sizeof(dib_header) + SIZE * SIZE / 8;
    header.unused = 0;
    header.pixel_array_offset = sizeof(header) + sizeof(dib_header);
    dib_header.header_size = sizeof(dib_header);
    dib_header.width=SIZE;
    dib_header.height=SIZE;
    dib_header.color_planes=1;
    dib_header.bits_per_pixel=1;
    dib_header.compression=0;
    dib_header.image_size=SIZE * SIZE / 8;
    dib_header.x_resolution=0;
    dib_header.y_resolution=0;
    dib_header.colors=0;
    dib_header.used_colors=0;
    palette.color1 = 0x000000;
    palette.color0 = 0xFFFFFF;

    int row_size = dib_header.width;
    int padding = (4 - (row_size % 4)) % 4; // padding to ensure each row is a multiple of 4 bytes
    int data_size = (row_size + padding) * dib_header.height;
    printf("%d\n", data_size);
    char* pixel_data = (char*)malloc(data_size);

    int f = open("image.bmp", O_CREAT | O_TRUNC | O_WRONLY, S_IWUSR | S_IRUSR);
    if(f < 0){
        write(2, "File Error!", 11);
        exit(1);
    }

    write(f, &header, sizeof(header));
    write(f, &dib_header, sizeof(dib_header));
    write(f, &palette, sizeof(palette));
    write(f, &pixel_data, sizeof(pixel_data));
    free(pixel_data);
    close(f);
}

int main(){
    int* values = (int*) malloc(SIZE*sizeof(int));
    for(int i=0; i<SIZE; ++i){
        values[i] = random()%SIZE;
        printf("%d ", values[i]);
    }
    puts("");
    char* bmp = (char*)calloc(file_size, 1);
    generate_bmp(bmp, file_size, values);
    
    free(values);
    
    return 0;
}