#include <stdlib.h>

#include <config.h>
#include <world.h>
#include <ray.h>

/// Returns an empty world with no light and no objects
World world_new(size_t max_groups, size_t max_lights)
{
    Group *groups = malloc(max_groups * sizeof(Group));
    PointLight *lights = malloc(max_lights * sizeof(PointLight));
    return (World) { 0, lights, 0, groups };
}

void world_free(World *world) {
    free(world->groups);
    free(world->lights);
}

void world_add_shape(World *w, Shape *s)
{
    Group g = group_new(mat4d_identity(), 1, s, s->name);
    world_add_group(w, g);
}

void world_add_group(World *w, Group g)
{
    w->groups[w->group_count++] = g;
}

void world_add_light(World *w, PointLight light) {
    w->lights[w->light_count++] = light;
}

int is_point_shadowed(Vec4D point, PointLight light, World world)
{
    Vec4D v = d4_sub(light.position, point);
    double distance = d4_mag(v);
    Vec4D direction = d4_norm(v);

    Ray r = (Ray) { point, direction };
    Intersection h = ray_intersect_world(r, world);
    return h.t < distance;
}
