typedef struct {
    float4 inv_transform[4];
    float field_of_view;
    int hsize;
    int vsize;
    int pad;  // To ensure aligned to 16 bytes
} Camera;

typedef struct {
    float4 color;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
    float reflective;
    float transparency;
    float refractive_index;
    float pad;
} Material;

typedef struct {
    int type;
    float ymin;   // Only relevant for cylinders and cones
    float ymax;   // ditto
    bool closed;  // ditto
    float4 inv_transform[4];
    Material material;
} Shape;

typedef struct {
    float4 origin;
    float4 direction;
} Ray;

float4 mat_mul_vec(__constant float4 mat[4], float4 vec) {
    return (float4)(
        dot(mat[0], vec),
        dot(mat[1], vec),
        dot(mat[2], vec),
        dot(mat[3], vec)
    );
}

__kernel void raytrace_kernel(
    __constant Camera *camera,
    __write_only image2d_t result_img
) {
    // Get pixel coordinates
    int pixel_x = get_global_id(0);
    int pixel_y = get_global_id(1);
    int2 pixel = (int2)(pixel_x, pixel_y);
    float2 pixelf = convert_float2(pixel);

    // Compute camera properties (common to every pixel)
    float half_view = tan(0.5f * camera->field_of_view);
    float aspect = (float)camera->hsize / (float)camera->vsize;
    float2 camera_half_size = aspect >= 1.0f
        ? (float2)(half_view, half_view / aspect)
        : (float2)(half_view * aspect, half_view); 
    float camera_pixel_size = camera_half_size.x * 2.0f / (float)camera->hsize;

    // Compute ray at this pixel
    float2 offset = (pixelf + 0.5f) * camera_pixel_size;   // Offset from edge of camera to pixel's center
    float4 pixel_center_view = (float4)(camera_half_size - offset, -1.0f, 1.0f); // Untransformed coordinates of pixel center in view space
    float4 pixel_center_world = mat_mul_vec(camera->inv_transform, pixel_center_view);
    float4 camera_origin_world = mat_mul_vec(camera->inv_transform, (float4)(0.0f, 0.0f, 0.0f, 1.0f));
    float4 direction = normalize(pixel_center_world - camera_origin_world);
    Ray ray = { camera_origin_world, direction };

    float4 color = (float4)(ray.direction.xyz, 1.0f);
    write_imagef(result_img, pixel, color);
}