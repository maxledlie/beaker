#define _USE_MATH_DEFINES
#define _DEFAULT_SOURCE

#include <math.h>

#include <camera.h>

Mat4D view_transform(Vec4D from, Vec4D to, Vec4D up) {
    Vec4D forward = d4_norm(d4_sub(to, from));
    Vec4D upn = d4_norm(up);
    Vec4D left = d4_cross(forward, upn);
    Vec4D true_up = d4_cross(left, forward);

    Mat4D orientation = mat4d_new((double []) {
        left.x, left.y, left.z, 0.0,
        true_up.x, true_up.y, true_up.z, 0.0,
        -forward.x, -forward.y, -forward.z, 0.0,
        0.0, 0.0, 0.0, 1.0
    });

    Mat4D result = mat4d_mul_mat4d(orientation, translation(-from.x, -from.y, -from.z));
    return result;
}

Camera camera_new(
    int hsize,
    int vsize,
    double field_of_view,
    Mat4D transform
) {
    double half_view = tan(0.5 * field_of_view);
    double aspect = (double) hsize / (double) vsize;
    
    double half_width;
    double half_height;
    if (aspect >= 1.0) {
        half_width = half_view;
        half_height = half_view / aspect;
    } else {
        half_width = half_view * aspect;
        half_height = half_view;
    }
    double pixel_size = half_width * 2 / (double)hsize;

    Mat4D inv_transform = mat4d_inverse(transform);

    return (Camera) {
        hsize,
        vsize,
        transform,
        inv_transform,
        half_width,
        half_height,
        pixel_size,
        field_of_view
    };
}

Camera camera_default() {
    return camera_new(
        160,
        120,
        0.5 * M_PI,
        mat4d_identity()
    );
}