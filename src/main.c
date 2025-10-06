#define _USE_MATH_DEFINES  // For M_PI on Windows
#define _DEFAULT_SOURCE    // For M_PI on Unix
#include <math.h>

#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <config.h>
#include <color.h>
#include <canvas.h>
#include <matrix.h>
#include <ray.h>
#include <lighting.h>

// Config
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


    // Create a group describing a square made of cylinders and spheres
    material = material_default();
    transform = mat4d_identity();
    transform = translation(-5.0, 0.0, 0.0);
    Shape left = cylinder_new(transform, material, "window_left", -5.0, -5.0, 1);

    transform = translation(5.0, 0.0, 0.0);
    Shape right = cylinder_new(transform, material, "window_right", -5.0, 5.0, 1);

    transform = mat4d_mul_mat4d(translation(0.0, 5.0, 0.0), rotation_z(M_PI / 4.0));
    Shape top = cylinder_new(transform, material, "window_top", -5.0, 5.0, 1);

    transform = mat4d_mul_mat4d(translation(0.0, -5.0, 0.0), rotation_z(M_PI / 4.0));
    Shape bottom = cylinder_new(transform, material, "window_top", -5.0, 5.0, 1);

    Vec4D light_position = d4_point(-2.0, 10.0, -10.0);
    Color light_color = (Color) {1.0, 1.0, 1.0};
    PointLight light = (PointLight) { light_position, light_color };

    World world = world_new();
    world.object_count = 4;
    world.objects = malloc(world.object_count * sizeof(Shape));
    world.objects[0] = left;
    world.objects[1] = right;
    world.objects[2] = top;
    world.objects[3] = bottom;
    world.light_count = 1;
    world.lights = malloc(world.light_count * sizeof(PointLight));
    world.lights[0] = light;

    Mat4D view = view_transform(
        d4_point(0.0, 2.0, -8.0),
        d4_point(0., 0., 0.),
        d4_vector(0., 1., 0.)
    );
    Camera camera = camera_new(1200, 900, M_PI / 3., view);
    Canvas canvas = canvas_create(camera.hsize, camera.vsize);

    log_line("Completed scene configuration");

    if (CFG_SINGLE_PIXEL_DEBUG) {
        printf("Debugging single pixel at (%d, %d)\n", CFG_SINGLE_PIXEL_X, CFG_SINGLE_PIXEL_Y);
        Ray ray = ray_at_pixel(camera, CFG_SINGLE_PIXEL_X, CFG_SINGLE_PIXEL_Y);
        Color c = ray_color(ray, world, CFG_RECURSION_DEPTH);
        printf("Output color: (%f, %f, %f)\n", c.r, c.g, c.b);
        return 0;
    }

    // Render
    log_line("Starting render");
    for (int y = 0; y < camera.vsize; y++) {
        for (int x = 0; x < camera.hsize; x++) {
            Ray ray = ray_at_pixel(camera, x, y);
            Color c = ray_color(ray, world, CFG_RECURSION_DEPTH);
            canvas_pixel_set(canvas, x, y, c);
        }
    }
    log_line("Completed render");
    canvas_save_ppm(canvas, "out.ppm");

    // Free stuff to keep address sanitizer happy
    free(world.lights);
    free(world.objects);
    canvas_destroy(canvas);
}