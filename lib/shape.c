#include <stdio.h>
#include <string.h>
#include <math.h>

#include <matrix.h>
#include <shape.h>

Shape _shape_new(int type, Mat4D transform, Material material, const char *name) {
    Mat4D inv = mat4d_inverse(transform);
    Shape s = { type, transform, inv, material, { 0 } };
    strncpy(s.name, name, SHAPE_NAME_LEN - 1);
    s.name[SHAPE_NAME_LEN - 1] = '\0';  // Ensure null-termination
    return s;
}

Shape sphere_new(Mat4D transform, Material material, char *name) {
    return _shape_new(SHAPE_SPHERE, transform, material, name);
}

Shape sphere_default() {
    return sphere_new(mat4d_identity(), material_default(), "default_sphere");
}

Shape plane_new(Mat4D transform, Material material, char *name) {
    return _shape_new(SHAPE_PLANE, transform, material, name);
}

Shape cube_new(Mat4D transform, Material material, char *name)
{
    return _shape_new(SHAPE_CUBE, transform, material, name);
}

Vec4D _sphere_normal(Vec4D object_point) {
    return d4_sub(object_point, d4_point(0, 0, 0));
}

Vec4D _plane_normal() {
    return d4_vector(0., 1., 0.);
}

Vec4D _cube_normal(Vec4D object_point) {
    double maxc = fmax(fabs(object_point.x), fabs(object_point.y));
    maxc = fmax(maxc, fabs(object_point.z));

    if (maxc == object_point.x) {
        return d4_vector(1.0, 0.0, 0.0);
    } else if (maxc == object_point.y) {
        return d4_vector(0.0, 1.0, 0.0);
    }
    return d4_vector(0.0, 0.0, 1.0);
}

Vec4D shape_normal(Shape *shape, Vec4D world_point)
{
    Mat4D inv_transpose = mat4d_transpose(shape->inv_transform);

    // Convert the point to object space
    Vec4D object_point = mat4d_mul_vec4d(shape->inv_transform, world_point);

    Vec4D object_normal;
    switch (shape->type) {
        case SHAPE_SPHERE:
            object_normal = _sphere_normal(object_point);
            break;
        case SHAPE_CUBE:
            object_normal = _cube_normal(object_point);
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
    Vec4D object_point = mat4d_mul_vec4d(shape.inv_transform, world_point);
    Vec4D pattern_point = mat4d_mul_vec4d(shape.material.pattern.inv_transform, object_point);
    return pattern_color_at(shape.material.pattern, pattern_point);
}
