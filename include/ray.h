#pragma once

#include <stddef.h>
#include <vector.h>
#include <matrix.h>
#include <geometry.h>

typedef struct Ray {
    Vec4D origin; 
    Vec4D direction;
} Ray;

typedef struct Intersection {
    // TODO: Add pointer to object we intersected with
    double t;
    Sphere *object_ptr;
} Intersection;

typedef struct IntersectionList {
    size_t count;
    Intersection *items;
} IntersectionList;

/// Returns the ray that would result from applying the given tranformation to the given input ray.
Ray ray_transform(Ray ray, Mat4D transform);

/// Returns the point the given distance along the ray.
Vec4D ray_position(Ray ray, double t);

/// Returns the t-values at which the given ray intersects a unit sphere at the origin.
IntersectionList ray_intersect_sphere(Ray ray, Sphere sphere);

/// Returns the intersection with the smallest positive t-value,
/// or NULL if intersection list is empty or has only negative t-values.
Intersection *hit(IntersectionList intersections);
