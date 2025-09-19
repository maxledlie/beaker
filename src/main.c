#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <color.h>
#include <canvas.h>
#include <matrix.h>


int main() {
    // Matrix test
    Mat4D id = mat4d_identity();
    mat4d_dbg(id);

    double a_vals[] = {
        1.0, 2.0, 3.0, 4.0,
        5.0, 6.0, 7.0, 8.0,
        9.0, 8.0, 7.0, 6.0,
        5.0, 4.0, 3.0, 2.0
    };
    Mat4D a = mat4d_new(a_vals);
    printf("\n");
    mat4d_dbg(a);

    double b_vals[] = {
        -2.0, 1.0, 2.0, 3.0,
        3.0, 2.0, 1.0, -1.0,
        4.0, 3.0, 6.0, 5.0,
        1.0, 2.0, 7.0, 8.0
    };
    Mat4D b = mat4d_new(b_vals);
    printf("\n");
    mat4d_dbg(a);

    Mat4D product = mat4d_mul_mat4d(a, b);
    printf("\n");
    mat4d_dbg(product);

    Canvas canvas = canvas_create(10, 10);
    Color red = { 1.0, 0.0, 0.0 };
    canvas_pixel_set(canvas, 0, 0, red);
    canvas_pixel_set(canvas, 1, 0, red);
    canvas_pixel_set(canvas, 0, 1, red);
    canvas_save_ppm(canvas, "out.ppm");
}