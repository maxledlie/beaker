#pragma once

#include <lighting.h>
#include <shape.h>

typedef struct World {
    size_t light_count;
    PointLight *lights;
    size_t group_count;
    Group *groups;
} World;

World world_new(size_t max_groups, size_t max_lights);
void world_free(World *world);
void world_add_shape(World *w, Shape *s);
void world_add_group(World *w, Group g);
void world_add_light(World *w, PointLight light);
int is_point_shadowed(Vec4D point, PointLight light, World world);