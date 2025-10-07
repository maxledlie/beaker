#pragma once

#include <matrix.h>

typedef struct {
    int hsize;
    int vsize;
    Mat4D transform;
    Mat4D inv_transform;
    double half_width;
    double half_height;
    double pixel_size;
    double field_of_view;
} Camera;

Camera camera_new(
    int hsize,
    int vsize,
    double field_of_view,
    Mat4D transform
);

Camera camera_default();
Mat4D view_transform(Vec4D from, Vec4D to, Vec4D up);