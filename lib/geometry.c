#include <matrix.h>
#include <geometry.h>

Sphere sphere_new() {
    Mat4D transform = mat4d_identity();
    return (Sphere) { transform, material_new() };
}

Vec4D sphere_normal(Sphere *sphere, Vec4D world_point)
{
    Mat4D inv = mat4d_inverse(sphere->transform);
    Mat4D inv_transpose = mat4d_transpose(inv);
    Vec4D object_point = mat4d_mul_vec4d(inv, world_point);
    Vec4D object_normal = d4_sub(object_point, d4_point(0, 0, 0));

    // TODO: Understand why the inverse transpose brings normal vectors
    // back to world space.
    Vec4D world_normal = mat4d_mul_vec4d(inv_transpose, object_normal);
    world_normal.w = 0.0;
    return d4_norm(world_normal);
}
