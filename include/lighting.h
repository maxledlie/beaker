#pragma once

#include <math.h>

#include <vector.h>
#include <material.h>

typedef struct PointLight {
    Vec4D position;
    Color intensity;
} PointLight;

Color lighting_compute(Material material, PointLight light, Vec4D point, Vec4D eye, Vec4D normal);


