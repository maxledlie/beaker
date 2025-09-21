#pragma once

#include <matrix.h>
#include <material.h>

typedef struct Sphere {
    Mat4D transform;
    Material material;
} Sphere;

Sphere sphere_new();

Vec4D sphere_normal(Sphere sphere, Vec4D world_point);
