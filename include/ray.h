#pragma once

#include <stddef.h>
#include <vector.h>
#include <matrix.h>
#include <shape.h>
#include <world.h>
#include <camera.h>

// ----------------------------------
// Intersection management
// ----------------------------------

typedef struct {
    double t;
    Shape *object_ptr;
} Intersection;

typedef struct {
    size_t count;
    size_t capacity;
    Intersection *items;
} IntersectionList;

IntersectionList intersection_list_new();
int intersection_list_add(IntersectionList *xs, Intersection x);

typedef struct {
    double t;
    Shape *object_ptr;
    Vec4D point;
    Vec4D over_point;
    Vec4D eyev;
    Vec4D normalv;
    int inside;
    Vec4D reflectv;
} IntersectionData;

// ----------------------------------
// Rays
// ----------------------------------

typedef struct Ray {
    Vec4D origin; 
    Vec4D direction;
} Ray;

Ray ray_at_pixel(Camera camera, int px, int py);

IntersectionData ray_prepare_computations(Ray r, Intersection i);

/// Returns the ray that would result from applying the given transformation to the given input ray.
Ray ray_transform(Ray ray, Mat4D transform);

/// Returns the point the given distance along the ray.
Vec4D ray_position(Ray ray, double t);

/// Returns the t-values at which the given ray intersects various objects.
IntersectionList ray_intersect_world(Ray ray, size_t object_count, Shape *objects);
IntersectionList ray_intersect_shape(Ray ray, Shape *shape);

/// Returns the intersection with the smallest positive t-value,
/// or NULL if intersection list is empty or has only negative t-values.
Intersection *hit(IntersectionList intersections);

Color ray_color(
    Ray ray,
    World world,
    int remaining_reflections
);
