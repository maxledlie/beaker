#define _USE_MATH_DEFINES
#include <math.h>

#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <color.h>
#include <canvas.h>
#include <matrix.h>
#include <ray.h>
#include <lighting.h>

const int RECURSION_DEPTH = 5;


void log_line(char *msg) {
    time_t raw_time = time(NULL);
    struct tm *utc_time = gmtime(&raw_time);
    printf("[%i:%i:%i] %s\n", 
        utc_time->tm_hour,
        utc_time->tm_min,
        utc_time->tm_sec,
        msg
    );
}


int main() {
    log_line("Starting scene configuration");

    Mat4D transform;
    Material material;

    material = material_new();
    transform = mat4d_identity();
    material.pattern = pattern_checker_new(color_rgb(0.8, 0.8, 0.9), color_rgb(0.2, 0.2, 0.3), mat4d_identity());
    material.reflective = 0.1;
    Shape floor = plane_new(transform, material);

    transform = mat4d_mul_mat4d(translation(0, 0, 5.), rotation_x(-M_PI / 2.));
    material = material_new();
    material.pattern = pattern_plain_new(color_rgb(0.4, 0.1, 0.1), mat4d_identity());
    material.diffuse = 0.6;
    Shape back_wall = plane_new(transform, material);

    transform = mat4d_mul_mat4d(rotation_y(-M_PI / 2.), rotation_x(-M_PI / 2.));
    transform = mat4d_mul_mat4d(translation(-4., 0., 0.), transform);
    material = material_new();
    material.pattern = pattern_plain_new(color_rgb(0.1, 0.4, 0.1), mat4d_identity());
    material.diffuse = 0.6;
    Shape left_wall = plane_new(transform, material);

    transform = mat4d_mul_mat4d(rotation_y(M_PI / 2.), rotation_x(-M_PI / 2.));
    transform = mat4d_mul_mat4d(translation(4., 0., 0.), transform);
    material = material_new();
    material.pattern = pattern_plain_new(color_rgb(0.1, 0.1, 0.4), mat4d_identity());
    material.diffuse = 0.6;
    Shape right_wall = plane_new(transform, material);

    transform = mat4d_mul_mat4d(translation(-0.5, 1.5, -1.0), scaling(1.5, 1.5, 1.5));
    material = material_new();
    material.ambient = 0.0;
    material.diffuse = 0.0;
    material.specular = 0.0;
    material.pattern = pattern_stripe_new(color_rgb(0.6, 0.2, 0.1), color_rgb(0.0, 0.2, 0.8), mat4d_identity());
    material.pattern.transform = mat4d_mul_mat4d(scaling(0.2, 0.2, 0.2), rotation_z(1.2));
    material.reflective = 1.0;
    Shape middle = sphere_new(transform, material);

    transform = mat4d_mul_mat4d(translation(1.5, 0.5, -2.9), scaling(0.5, 0.5, 0.5));
    material = material_new();
    material.pattern = pattern_plain_new(color_rgb(1.0, 0.5, 0.1), mat4d_identity());
    material.diffuse = 0.7;
    material.specular = 0.6;
    material.shininess = 500;
    material.reflective = 0.1;
    Shape right = sphere_new(transform, material);

    transform = mat4d_mul_mat4d(translation(-2.0, 0.6, -2.5), scaling(0.6, 0.6, 0.6));
    material = material_new();
    material.pattern = pattern_plain_new(color_rgb(1.0, 0.8, 0.1), mat4d_identity());
    material.diffuse = 0.7;
    material.specular = 0.3;
    material.reflective = 0.1;
    Shape left = sphere_new(transform, material);

    Vec4D light_position = d4_point(-4.0, 10.0, -10.0);
    Color light_color = (Color) {1.0, 1.0, 1.0};
    PointLight light = (PointLight) { light_position, light_color };

    World world = world_new();
    world.object_count = 7;
    world.objects = malloc(world.object_count * sizeof(Shape));
    world.objects[0] = floor;
    world.objects[1] = middle;
    world.objects[2] = right;
    world.objects[3] = left;
    world.objects[4] = back_wall;
    world.objects[5] = left_wall;
    world.objects[6] = right_wall;
    world.light_count = 1;
    world.lights = malloc(world.light_count * sizeof(PointLight));
    world.lights[0] = light;

    Mat4D view = view_transform(
        d4_point(0, 1.5, -7.5),
        d4_point(0., 1., 0.),
        d4_vector(0., 1., 0.)
    );
    Camera camera = camera_new(1000, 800, M_PI / 3., view);
    Canvas canvas = canvas_create(camera.hsize, camera.vsize);

    log_line("Completed scene configuration");

    // Render
    log_line("Starting render");
    for (int y = 0; y < camera.vsize; y++) {
        for (int x = 0; x < camera.hsize; x++) {
            Ray ray = ray_at_pixel(camera, x, y);
            Color c = ray_color(ray, world, RECURSION_DEPTH);
            canvas_pixel_set(canvas, x, y, c);
        }
    }
    log_line("Completed render");
    canvas_save_ppm(canvas, "out.ppm");
}