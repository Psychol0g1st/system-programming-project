#include <stdio.h>
#include <stdint.h>

#define WIDTH 5
#define HEIGHT 5

typedef struct Palette{
    unsigned int color0;
    unsigned int color1;
}Palette;

int main() {
    Palette palette;
    uint8_t image[HEIGHT][WIDTH]; // 5x5 pixel array
    uint32_t filesize = 54 + (WIDTH * HEIGHT / 8); // size of file in bytes
    uint32_t offset = 54; // offset to pixel data
    uint32_t header_size = 40; // size of info header
    uint32_t planes = 1; // number of color planes
    uint32_t bits_per_pixel = 1; // number of bits per pixel
    uint32_t compression = 0; // no compression
    uint32_t image_size = WIDTH * HEIGHT / 8; // size of image in bytes
    uint32_t x_resolution = 2835; // horizontal resolution in pixels/meter (72 dpi)
    uint32_t y_resolution = 2835; // vertical resolution in pixels/meter (72 dpi)
    uint32_t num_colors = 0; // number of colors in the color palette
    uint32_t important_colors = 0; // number of important colors in the color palette
palette.color1 = 0x000000;
    palette.color0 = 0xFFFFFF;
    // initialize image data
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            image[y][x] = (x + y) % 2; // alternating black and white pixels
        }
    }

    // write bmp header
    const int width = WIDTH;
    const int height = HEIGHT;
    FILE* fp = fopen("image.bmp", "wb");
    fwrite("BM", 1, 2, fp); // BMP signature
    fwrite(&filesize, 4, 1, fp); // file size
    fwrite("\0\0\0\0", 4, 1, fp); // reserved
    fwrite(&offset, 4, 1, fp); // offset to pixel data
    fwrite(&header_size, 4, 1, fp); // size of info header
    fwrite(&width, 4, 1, fp); // image width
    fwrite(&height, 4, 1, fp); // image height
    fwrite(&planes, 2, 1, fp); // number of color planes
    fwrite(&bits_per_pixel, 2, 1, fp); // number of bits per pixel
    fwrite(&compression, 4, 1, fp); // compression method
    fwrite(&image_size, 4, 1, fp); // size of image data
    fwrite(&x_resolution, 4, 1, fp); // horizontal resolution
    fwrite(&y_resolution, 4, 1, fp); // vertical resolution
    fwrite(&num_colors, 4, 1, fp); // number of colors in color palette
    fwrite(&important_colors, 4, 1, fp); // number of important colors in color palette
    fwrite(&palette, 8,1,fp);
    // write pixel data
    for (int y = HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < WIDTH; x++) {
            fwrite(&image[y][x], 1, 1, fp); // write one byte per pixel
        }
    }

    fclose(fp);
    return 0;
}





