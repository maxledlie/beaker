#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <color.h>
#include <canvas.h>
#include <matrix.h>
#include <ray.h>


int main() {
    Vec4D ray_origin = d4_point(0., 0., -5.);
    double wall_z = 10.0;
    double wall_size = 7.0;

    int canvas_pixels = 100;
    double pixel_size = wall_size / canvas_pixels;
    double half = wall_size / 2.0;

    Mat4D transform = translation(0.5, 0.0, 0.0);

    Canvas canvas = canvas_create(canvas_pixels, canvas_pixels);
    Color red = { 1.0, 0.0, 0.0 };

    Sphere sphere = sphere_new();
    sphere.transform = transform;

    for (int y = 0; y < canvas_pixels; y++) {
        double world_y = half - pixel_size * y;
        for (int x = 0; x < canvas_pixels; x++) {
                double world_x = -half + pixel_size * x;
                Vec4D position = d4_point(world_x, world_y, wall_z);
                Ray r = { ray_origin, d4_norm(d4_sub(position, ray_origin)) };
                IntersectionArray xs = ray_intersect_sphere(r, sphere);

                if (hit(xs)) {
                    canvas_pixel_set(canvas, x, y, red);
                }
            }
    }
    canvas_save_ppm(canvas, "out.ppm");
}