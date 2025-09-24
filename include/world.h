#include <lighting.h>
#include <geometry.h>

typedef struct World {
    size_t light_count;
    PointLight *lights;
    size_t object_count;
    Sphere *objects;
} World;

World world_new();
World world_default();