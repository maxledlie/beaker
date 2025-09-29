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

    Shape floor = plane_new();
    floor.material.pattern = pattern_checker_new(color_rgb(0.8, 0.8, 0.9), color_rgb(0.2, 0.2, 0.3));
    floor.material.reflective = 0.1;

    Shape back_wall = plane_new();
    back_wall.transform = mat4d_mul_mat4d(translation(0, 0, 5.), rotation_x(-M_PI / 2.));
    back_wall.material.pattern = pattern_plain_new(color_rgb(0.4, 0.1, 0.1));
    back_wall.material.diffuse = 0.6;

    Shape left_wall = plane_new();
    left_wall.transform = mat4d_mul_mat4d(rotation_y(-M_PI / 2.), rotation_x(-M_PI / 2.));
    left_wall.transform = mat4d_mul_mat4d(translation(-4., 0., 0.), left_wall.transform);
    left_wall.material.pattern = pattern_plain_new(color_rgb(0.1, 0.4, 0.1));
    left_wall.material.diffuse = 0.6;

    Shape right_wall = plane_new();
    right_wall.transform = mat4d_mul_mat4d(rotation_y(M_PI / 2.), rotation_x(-M_PI / 2.));
    right_wall.transform = mat4d_mul_mat4d(translation(4., 0., 0.), right_wall.transform);
    right_wall.material.pattern = pattern_plain_new(color_rgb(0.1, 0.1, 0.4));
    right_wall.material.diffuse = 0.6;

    Shape middle = sphere_new();
    middle.transform = mat4d_mul_mat4d(translation(-0.5, 1.5, -1.0), scaling(1.5, 1.5, 1.5));
    middle.material.ambient = 0.0;
    middle.material.diffuse = 0.0;
    middle.material.specular = 0.0;
    middle.material.pattern = pattern_stripe_new(color_rgb(0.6, 0.2, 0.1), color_rgb(0.0, 0.2, 0.8));
    middle.material.pattern.transform = mat4d_mul_mat4d(scaling(0.2, 0.2, 0.2), rotation_z(1.2));
    middle.material.reflective = 1.0;

    Shape right = sphere_new();
    right.transform = mat4d_mul_mat4d(translation(1.5, 0.5, -2.9), scaling(0.5, 0.5, 0.5));
    right.material = material_new();
    right.material.pattern = pattern_plain_new(color_rgb(1.0, 0.5, 0.1));
    right.material.diffuse = 0.7;
    right.material.specular = 0.6;
    right.material.shininess = 500;
    right.material.reflective = 0.1;

    Shape left = sphere_new();
    left.transform = mat4d_mul_mat4d(translation(-2.0, 0.6, -2.5), scaling(0.6, 0.6, 0.6));
    left.material = material_new();
    left.material.pattern = pattern_plain_new(color_rgb(1.0, 0.8, 0.1));
    left.material.diffuse = 0.7;
    left.material.specular = 0.3;
    left.material.reflective = 0.1;

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