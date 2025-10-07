#include <stdlib.h>

#include <config.h>
#include <world.h>
#include <ray.h>

/// Returns an empty world with no light and no objects
World world_new()
{
    return (World) { 0, NULL, 0, NULL };
}

/// Returns a placeholder world for testing.
/// Contains a single white point light at (-10, 10, -10) and two concentric spheres at the origin with radii 0.5 and 1.
World world_default()
{
    PointLight *lights = malloc(sizeof(PointLight));
    lights[0] = (PointLight) { d4_point(-10., 10., -10.), color_rgb(1., 1., 1.) };

    Shape *objects = malloc(2 * sizeof(Shape));
    Material material = material_default();
    material.pattern = pattern_plain_new(color_rgb(0.8, 1.0, 0.6), mat4d_identity());
    material.diffuse = 0.7;
    material.specular = 0.2;
    objects[0] = sphere_new(mat4d_identity(), material, "sphere_outer");

    objects[1] = sphere_new(scaling(0.5, 0.5, 0.5), material_default(), "sphere_inner");

    return (World) { 1, lights, 2, objects};
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
