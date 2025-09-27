#pragma once

#include <matrix.h>
#include <material.h>

#define SHAPE_SPHERE 0
#define SHAPE_PLANE 1

typedef struct {
    int type;
    Mat4D transform;
    Material material;
} Shape;

Shape sphere_new();
Shape plane_new();

Vec4D shape_normal(Shape *shape, Vec4D world_point);
Color shape_color_at(Shape shape, Vec4D world_point);
