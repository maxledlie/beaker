#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char BYTE;

struct Color {
    BYTE r;
    BYTE g;
    BYTE b;
};

struct Canvas {
    int width;
    int height;
    struct Color *pixels;
};

struct Canvas new_canvas(int width, int height) {
    /// Initialises a canvas of the requested width and height with all pixels set to black.
    int size = width * height * sizeof(struct Color);
    struct Color *p = malloc(size);
    memset(p, 0, size);

    struct Canvas ret;
    ret.width = width;
    ret.height = height;
    ret.pixels = p;
    return ret;
}


int write_ppm(struct Canvas canvas, const char *filepath) {
    FILE *f = fopen(filepath, "w");
    if (!f) {
        perror("Failed to open file");
        return 1;
    }

    fprintf(f, "P3\n");                                  // PPM identifier
    fprintf(f, "%d %d\n", canvas.width, canvas.height);  // Width and height in pixels
    fprintf(f, "255\n");                                 // Maximum color value

    for (int y = 0; y < canvas.height; y++) {
        for (int x = 0; x < canvas.width; x++) {
            struct Color c = canvas.pixels[y * canvas.width + x];
            fprintf(f, "%d %d %d ", c.r, c.g, c.b);
        }
        fprintf(f, "\n");
    }
    return 0;
}

int main() {
    struct Canvas canvas = new_canvas(10, 10);
    write_ppm(canvas, "out.ppm");
}