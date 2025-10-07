typedef struct {
    float4 inv_transform[4];
    float field_of_view;
    float pad[3];  // To ensure aligned to 16 bytes
} Camera;

typedef struct {
    float4 origin;
    float4 direction;
} Ray;

float4 mat_mul_vec(float4 mat[4], float4 vec) {
    return (float4)(
        dot(mat[0], vec),
        dot(mat[1], vec),
        dot(mat[2], vec),
        dot(mat[3], vec)
    );
}

// Ultra-simple kernel to get started.
// We provide a number of spheres, and the x-position, y-position, and radius
// of each sphere. The kernel should output, for each pixel, either 0.0 if the
// point is within a circle, or 1.0 if it is not.
__kernel void circles_kernel(
    __constant Camera *camera,
    const uint num_circles,
    const uint img_width,
    __global const float *circle_xs,
    __global const float *circle_ys,
    __global const float *circle_radii,
    __write_only image2d_t result_img
) {
    // Get pixel coordinates
    int pixel_x = get_global_id(0);
    int pixel_y = get_global_id(1);
    int2 pixel = (int2)(pixel_x, pixel_y);
    float2 pixelf = convert_float2(pixel);

    // // Compute camera properties
    // // In theory we could do this just once on the CPU, but would have more to pass in.
    // float half_view = tan(0.5f * camera.field_of_view);

    // // Compute the ray at this pixel
    // float2 offset = (pixelf + (float2)(0.5f, 0.5f)) * pixel_size;

    float4 color = (float4)(0.0f, 0.0f, 0.0f, 1.0f);
    for (int i = 0; i < num_circles; i++) {
        float2 circle = (float2)(circle_xs[i], circle_ys[i]);
        float radius = circle_radii[i];
        if (distance(circle, pixelf) <= radius) {
            color = (float4)(1.0f, 0.0f, 0.0f, 1.0f);
        }
    }
    
    write_imagef(result_img, pixel, color);
}