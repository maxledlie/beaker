#include <renderer.h>
#include <ray.h>
#include <config.h>

int render_image(World world, Camera camera, Canvas canvas) {
    for (int y = 0; y < camera.vsize; y++) {
        for (int x = 0; x < camera.hsize; x++) {
            Ray ray = ray_at_pixel(camera, x, y);
            Color c = ray_color(ray, world, CFG_RECURSION_DEPTH);
            canvas_pixel_set(canvas, x, y, c);
        }
    }
    return 0;
}