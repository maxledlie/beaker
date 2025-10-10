// Ultra-simple kernel to get started.
// We provide a number of spheres, and the x-position, y-position, and radius
// of each sphere. The kernel should output, for each pixel, either 0.0 if the
// point is within a circle, or 1.0 if it is not.
__kernel void circles_kernel(
    __global const uint *num_circles,
    __global const float *circle_xs,
    __global const float *circle_ys,
    __global const float *circle_radii,
    __global const uint *img_width,
    __global float *result
) {
    int index = get_global_id(0);
    int pixel_x = index % *img_width;
    int pixel_y = index / *img_width;
    float2 pixel = (float2)(pixel_x, pixel_y);

    float in_circle = 0.0f;
    for (int i = 0; i < *num_circles; i++) {
        float2 circle = (float2)(circle_xs[i], circle_ys[i]);
        float radius = circle_radii[i];
        if (distance(circle, pixel) <= radius) {
            in_circle = 1.0f;
        }
    }
    
    result[index] = in_circle;
}