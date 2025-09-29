#pragma once

#include <matrix.h>
#include <material.h>

#define SHAPE_SPHERE 0
#define SHAPE_PLANE 1

typedef struct {
    int type;
    Mat4D transform;
    Mat4D inv_transform;
    Material material;
} Shape;

Shape sphere_new(Mat4D transform, Material material);
Shape plane_new(Mat4D transform, Material material);

Vec4D shape_normal(Shape *shape, Vec4D world_point);
Color shape_color_at(Shape shape, Vec4D world_point);
