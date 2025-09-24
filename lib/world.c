#include <stdlib.h>

#include <world.h>

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

    Sphere *objects = malloc(2 * sizeof(Sphere));
    objects[0] = sphere_new();
    objects[0].material.color = color_rgb(0.8, 1.0, 0.6);
    objects[0].material.diffuse = 0.7;
    objects[0].material.specular = 0.2;

    objects[1] = sphere_new();
    objects[1].transform = scaling(0.5, 0.5, 0.5);

    return (World) { 1, lights, 2, objects};
}
