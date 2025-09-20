#pragma once

#include <matrix.h>

typedef struct Sphere {
    Mat4D transform;
} Sphere;

Sphere sphere_new();

Vec4D sphere_normal(Sphere sphere, Vec4D world_point);
