#include <canvas.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

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

void canvas_pixel_set(Canvas canvas, int x, int y, Color color) {
    canvas.pixels[y * canvas.width + x] = color;
}

Color canvas_pixel_get(Canvas canvas, int x, int y) {
    return canvas.pixels[y * canvas.width + x];
}

int serialize_intensity(double intensity) {
    double i = fmin(fmax(intensity, 0.0), 1.0);
    return (int) floor(i * 255);
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
            int r = serialize_intensity(c.r);
            int g = serialize_intensity(c.g);
            int b = serialize_intensity(c.b);
            fprintf(f, "%d %d %d ", r, g, b);
        }
        fprintf(f, "\n");
    }
    fclose(f);
    return 0;
}
