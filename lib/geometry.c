#include <stdio.h>

#include <matrix.h>
#include <geometry.h>

Shape _shape_new(int type) {
    Mat4D transform = mat4d_identity();
    return (Shape) { type, transform, material_new() };
}

Shape sphere_new() {
    return _shape_new(SHAPE_SPHERE);
}

Shape plane_new() {
    return _shape_new(SHAPE_PLANE);
}

Vec4D _sphere_normal(Vec4D object_point) {
    return d4_sub(object_point, d4_point(0, 0, 0));
}

Vec4D _plane_normal() {
    return d4_vector(0., 1., 0.);
}

Vec4D shape_normal(Shape *shape, Vec4D world_point)
{
    Mat4D inv = mat4d_inverse(shape->transform);
    Mat4D inv_transpose = mat4d_transpose(inv);

    // Convert the point to object space
    Vec4D object_point = mat4d_mul_vec4d(inv, world_point);

    Vec4D object_normal;
    switch (shape->type) {
        case SHAPE_SPHERE:
            object_normal = _sphere_normal(object_point);
            break;
        case SHAPE_PLANE:
            object_normal = _plane_normal();
            break;
        default:
            printf("Unrecognised shape type %i", shape->type);
    }

    // Convert the normal back to world space
    Vec4D world_normal = mat4d_mul_vec4d(inv_transpose, object_normal);
    world_normal.w = 0.0;
    return d4_norm(world_normal);
}

Color shape_color_at(Shape shape, Vec4D world_point)
{
    Vec4D object_point = mat4d_mul_vec4d(mat4d_inverse(shape.transform), world_point);
    Vec4D pattern_point = mat4d_mul_vec4d(mat4d_inverse(shape.material.pattern.transform), object_point);
    return pattern_color_at(shape.material.pattern, pattern_point);
}
