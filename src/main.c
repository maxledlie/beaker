#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <color.h>
#include <canvas.h>
#include <matrix.h>
#include <ray.h>


int main() {
    // // Matrix test
    // Mat4D id = mat4d_identity();
    // mat4d_dbg(id);

    // double a_vals[] = {
    //     1.0, 2.0, 3.0, 4.0,
    //     5.0, 6.0, 7.0, 8.0,
    //     9.0, 8.0, 7.0, 6.0,
    //     5.0, 4.0, 3.0, 2.0
    // };
    // Mat4D a = mat4d_new(a_vals);
    // printf("\n");
    // mat4d_dbg(a);

    // double b_vals[] = {
    //     -2.0, 1.0, 2.0, 3.0,
    //     3.0, 2.0, 1.0, -1.0,
    //     4.0, 3.0, 6.0, 5.0,
    //     1.0, 2.0, 7.0, 8.0
    // };
    // Mat4D b = mat4d_new(b_vals);
    // printf("\n");
    // mat4d_dbg(a);

    // Mat4D product = mat4d_mul_mat4d(a, b);
    // printf("\n");
    // mat4d_dbg(product);

    Vec4D ray_origin = d4_point(0., 0., -5.);
    double wall_z = 10.0;
    double wall_size = 20.0;

    int canvas_pixels = 100;
    double pixel_size = wall_size / canvas_pixels;
    double half = wall_size / 2.0;

    Canvas canvas = canvas_create(canvas_pixels, canvas_pixels);
    Color red = { 1.0, 0.0, 0.0 };

    for (int y = 0; y < canvas_pixels; y++) {
        double world_y = half - pixel_size * y;
        for (int x = 0; x < canvas_pixels; x++) {
                double world_x = -half + pixel_size * x;
                Vec4D position = d4_point(world_x, world_y, wall_z);
                Ray r = { ray_origin, d4_norm(d4_sub(position, ray_origin)) };
                IntersectionArray xs = ray_intersect_sphere(r);

                if (hit(xs)) {
                    canvas_pixel_set(canvas, x, y, red);
                }
            }
    }
    canvas_save_ppm(canvas, "out.ppm");
}