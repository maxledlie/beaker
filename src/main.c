#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <vec.h>
#include <color.h>

typedef unsigned char BYTE;

typedef struct Canvas {
    int width;
    int height;
    Color *pixels;
} Canvas;

struct Canvas new_canvas(int width, int height) {
    /// Initialises a canvas of the requested width and height with all pixels set to black.
    int size = width * height * sizeof(Color);
    Color *p = malloc(size);
    memset(p, 0, size);

    Canvas ret = {
        width,
        height,
        p
    };
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

    // TODO: No line more than 70 characters long
    for (int y = 0; y < canvas.height; y++) {
        for (int x = 0; x < canvas.width; x++) {
            struct Color c = canvas.pixels[y * canvas.width + x];
            fprintf(f, "%d %d %d ", c.r, c.g, c.b);
        }
        fprintf(f, "\n");
    }
    fclose(f);
    return 0;
}

int main() {
    struct Canvas canvas = new_canvas(10, 10);
    write_ppm(canvas, "out.ppm");
}