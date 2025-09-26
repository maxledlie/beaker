#define _USE_MATH_DEFINES
#include <math.h>

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
    Sphere floor = sphere_new();
    floor.transform = scaling(8., 0.01, 10.);
    floor.material = material_new();
    floor.material.color = color_rgb(1.0, 0.9, 0.9);
    floor.material.specular = 0.;

    Sphere middle = sphere_new();
    middle.transform = translation(-0.5, 1.0, 0.5);
    middle.material = material_new();
    middle.material.color = color_rgb(0.1, 1.0, 0.5);
    middle.material.diffuse = 0.7;
    middle.material.specular = 0.3;

    Sphere right = sphere_new();
    right.transform = mat4d_mul_mat4d(translation(1.5, 0.5, -0.5), scaling(0.5, 0.5, 0.5));
    right.material = material_new();
    right.material.color = color_rgb(1.0, 0.5, 0.1);
    right.material.diffuse = 0.7;
    right.material.specular = 0.3;

    Sphere left = sphere_new();
    left.transform = mat4d_mul_mat4d(translation(-1.2, 0.8, -0.75), scaling(0.33, 0.8, 0.33));
    left.material = material_new();
    left.material.color = color_rgb(1.0, 0.8, 0.1);
    left.material.diffuse = 0.7;
    left.material.specular = 0.3;

    Vec4D light_position = d4_point(-10.0, 10.0, -10.0);
    Color light_color = (Color) {1.0, 1.0, 1.0};
    PointLight light = (PointLight) { light_position, light_color };

    World world = world_new();
    world.object_count = 4;
    world.objects = malloc(world.object_count * sizeof(Sphere));
    world.objects[0] = floor;
    world.objects[1] = middle;
    world.objects[2] = right;
    world.objects[3] = left;
    world.light_count = 1;
    world.lights = malloc(world.light_count * sizeof(PointLight));
    world.lights[0] = light;

    Mat4D view = view_transform(
        d4_point(0, 1.5, -5.),
        d4_point(0., 1., 0.),
        d4_vector(0., 1., 0.)
    );
    Camera camera = camera_new(100, 80, M_PI / 3., view);
    Canvas canvas = canvas_create(camera.hsize, camera.vsize);

    // Render
    for (int y = 0; y < camera.vsize; y++) {
        for (int x = 0; x < camera.hsize; x++) {
            Ray ray = ray_at_pixel(camera, x, y);
            Color c = ray_color(ray, world);
            canvas_pixel_set(canvas, x, y, c);
        }
    }
    canvas_save_ppm(canvas, "out.ppm");
}