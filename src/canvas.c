#include <canvas.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

Canvas canvas_create(int width, int height) {
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

void canvas_destroy(Canvas canvas) {
    free(canvas.pixels);
}

int canvas_pixel_set(Canvas canvas, int x, int y, Color color) {
    canvas.pixels[y * canvas.width + x] = color;
}

Color canvas_pixel_get(Canvas canvas, int x, int y) {
    return canvas.pixels[y * canvas.width + x];
}

int canvas_save_ppm(Canvas canvas, const char *filepath) {
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
            Color c = canvas.pixels[y * canvas.width + x];
            int r = (int) floor(c.r * 255.0);
            int g = (int) floor(c.g * 255.0);
            int b = (int) floor(c.b * 255.0);
            fprintf(f, "%d %d %d ", r, g, b);
        }
        fprintf(f, "\n");
    }
    fclose(f);
    return 0;
}
