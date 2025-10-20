#include <renderer.h>
#include <ray.h>
#include <config.h>

Ray _random_ray_within_pixel(Camera camera, int px, int py)
{
    // Offset from edge of canvas to pixel's center
    double xoffset = (px + 0.5) * camera.pixel_size;
    double yoffset = (py + 0.5) * camera.pixel_size;

    // Untransformed coordinates of the pixel in world space.
    double world_x = camera.half_width - xoffset;
    double world_y = camera.half_height - yoffset;

    // Using the camera matrix, transform the canvas point and the origin,
    // and then compute the ray's direction vector.
    Mat4D inv = camera.inv_transform;
    Vec4D pixel = mat4d_mul_vec4d(inv, d4_point(world_x, world_y, -1));
    Vec4D origin = mat4d_mul_vec4d(inv, d4_point(0., 0., 0.));
    Vec4D direction = d4_norm(d4_sub(pixel, origin));
    
    return (Ray){ origin, direction };
}

int render_image(World world, Camera camera, Canvas canvas) {
    for (int y = 0; y < camera.vsize; y++) {
        for (int x = 0; x < camera.hsize; x++) {
            Color combined = color_black();
            for (int i = 0; i < CFG_NUM_SAMPLES; i++) {
                Ray ray = random_ray_within_pixel(camera, x, y);
                Color c = ray_color(ray, world, CFG_RECURSION_DEPTH);
                combined = color_add(combined, c);
            }
            combined = color_div(combined, CFG_NUM_SAMPLES);
            canvas_pixel_set(canvas, x, y, combined);
        }
    }
    return 0;
}