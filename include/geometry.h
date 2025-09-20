#pragma once

#include <matrix.h>

typedef struct Sphere {
    Mat4D transform;
} Sphere;

Sphere sphere_new();