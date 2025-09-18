#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <color.h>
#include <canvas.h>


int main() {
    Canvas canvas = canvas_create(10, 10);
    Color red = { 1.0, 0.0, 0.0 };
    canvas_pixel_set(canvas, 0, 0, red);
    canvas_pixel_set(canvas, 1, 0, red);
    canvas_pixel_set(canvas, 0, 1, red);
    canvas_save_ppm(canvas, "out.ppm");
}