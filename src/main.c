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

    int canvas_pixels = 512;
    double pixel_size = wall_size / canvas_pixels;
    double half = wall_size / 2.0;

    Canvas canvas = canvas_create(canvas_pixels, canvas_pixels);

    Sphere sphere = sphere_new();
    sphere.material = material_new();
    sphere.material.color = (Color) { 1.0, 0.2, 1.0 };

    Vec4D light_position = d4_point(-10.0, 10.0, -10.0);
    Color light_color = (Color) {1.0, 1.0, 1.0};
    PointLight light = (PointLight) { light_position, light_color };

    World world = world_new();
    world.object_count = 1;
    world.objects = malloc(sizeof(Sphere));
    world.objects[0] = sphere;
    world.light_count = 1;
    world.lights = malloc(sizeof(PointLight));
    world.lights[0] = light;

    for (int y = 0; y < canvas_pixels; y++) {
        double world_y = half - pixel_size * y;
        for (int x = 0; x < canvas_pixels; x++) {
            double world_x = -half + pixel_size * x;
            Vec4D position = d4_point(world_x, world_y, wall_z);
            Ray r = { ray_origin, d4_norm(d4_sub(position, ray_origin)) };
            Color c = ray_color(r, world.light_count, world.lights, world.object_count, world.objects);
            canvas_pixel_set(canvas, x, y, c);
        }
    }
    canvas_save_ppm(canvas, "out.ppm");
}