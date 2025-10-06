#pragma once

#include <matrix.h>
#include <material.h>

#define SHAPE_SPHERE   0
#define SHAPE_PLANE    1
#define SHAPE_CUBE     2
#define SHAPE_CYLINDER 3
#define SHAPE_CONE     4

#define SHAPE_NAME_LEN 64

typedef struct {
    int type;
    Mat4D transform;
    Mat4D inv_transform;
    Material material;
    char name[SHAPE_NAME_LEN];
    // For infinite shapes like cylinders and cones we can optionally provide minimum and maximum y-coordinates
    double ymin;
    double ymax;
    int closed;
} Shape;

Shape sphere_new(Mat4D transform, Material material, char *name);
Shape plane_new(Mat4D transform, Material material, char *name);
Shape cube_new(Mat4D transform, Material material, char *name);
Shape cylinder_new(Mat4D transform, Material material, char *name, double ymin, double ymax, int closed);
Shape cone_new(Mat4D transform, Material material, char *name, double ymin, double ymax, int closed);

Shape sphere_default();

Vec4D shape_normal(Shape *shape, Vec4D world_point);
Color shape_color_at(Shape shape, Vec4D world_point);
