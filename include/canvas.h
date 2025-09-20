#pragma once

#include <color.h>

typedef struct Canvas {
    int width;
    int height;
    Color *pixels;
} Canvas;

Canvas canvas_create(int width, int height);
void canvas_destroy(Canvas canvas);
void canvas_pixel_set(Canvas canvas, int x, int y, Color color);
Color canvas_pixel_get(Canvas canvas, int x, int y);

int canvas_save_ppm(Canvas canvas, const char* filepath);
