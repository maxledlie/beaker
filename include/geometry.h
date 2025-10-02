#pragma once

#include <matrix.h>
#include <material.h>

#define SHAPE_SPHERE 0
#define SHAPE_PLANE 1

#define SHAPE_NAME_LEN 64

typedef struct {
    int type;
    Mat4D transform;
    Mat4D inv_transform;
    Material material;
    char name[SHAPE_NAME_LEN];
} Shape;

Shape sphere_new(Mat4D transform, Material material, char *name);
Shape plane_new(Mat4D transform, Material material, char *name);
Shape sphere_default();
Vec4D shape_normal(Shape *shape, Vec4D world_point);
Color shape_color_at(Shape shape, Vec4D world_point);
