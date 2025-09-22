#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <color.h>
#include <canvas.h>
#include <matrix.h>
#include <ray.h>
#include <lighting.h>


int main() {
    Vec4D ray_origin = d4_point(0., 0., -5.);
    double wall_z = 10.0;
    double wall_size = 7.0;

    int canvas_pixels = 100;
    double pixel_size = wall_size / canvas_pixels;
    double half = wall_size / 2.0;

    Canvas canvas = canvas_create(canvas_pixels, canvas_pixels);

    Sphere sphere = sphere_new();
    sphere.material = material_new();
    sphere.material.color = (Color) { 1.0, 0.2, 1.0 };

    Vec4D light_position = d4_point(-10.0, 10.0, -10.0);
    Color light_color = (Color) {1.0, 1.0, 1.0};
    PointLight light = (PointLight) { light_position, light_color };

    for (int y = 0; y < canvas_pixels; y++) {
        double world_y = half - pixel_size * y;
        for (int x = 0; x < canvas_pixels; x++) {
                double world_x = -half + pixel_size * x;
                Vec4D position = d4_point(world_x, world_y, wall_z);
                Ray r = { ray_origin, d4_norm(d4_sub(position, ray_origin)) };
                IntersectionArray xs = ray_intersect_sphere(r, sphere);

                Intersection *h = hit(xs);
                if (h) {
                    Vec4D point = ray_position(r, h->t);
                    Vec4D normal = sphere_normal(sphere, point);
                    Vec4D eye = d4_neg(r.direction);
                    Color c = lighting_compute(sphere.material, light, point, eye, normal);
                    canvas_pixel_set(canvas, x, y, c);
                }
            }
    }
    canvas_save_ppm(canvas, "out.ppm");
}