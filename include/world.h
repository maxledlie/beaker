#pragma once

#include <lighting.h>
#include <shape.h>

typedef struct World {
    size_t light_count;
    PointLight *lights;
    size_t object_count;
    Shape *objects;
} World;

World world_new();
World world_default();
int is_point_shadowed(Vec4D point, PointLight light, World world);