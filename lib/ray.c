#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <vector.h>
#include <ray.h>
#include <geometry.h>

const size_t BASE_INTERSECTION_COUNT = 128;

/// Creates a new, dynamically sized, always-sorted, insert-only list of intersections
IntersectionList intersection_list_new() {
    Intersection *ptr = malloc(BASE_INTERSECTION_COUNT * sizeof(Intersection));
    memset(ptr, 0, BASE_INTERSECTION_COUNT * sizeof(Intersection));
    return (IntersectionList) { 0, BASE_INTERSECTION_COUNT, ptr};
}

int intersection_list_add(IntersectionList *xs, Intersection x)
{
    // Allocate additional memory if required
    if (xs->count >= xs->capacity) {
        xs->capacity *= 2;
        xs->items = realloc(xs->items, xs->capacity);
        if (!xs->items) {
            return 1;
        }
    }

    // Find the appropriate position for the intersection: t-values must be increasing.
    // OPT: A binary search for the correct insertion point may be more efficient when array gets large.
    size_t home = xs->count;
    for (size_t i = 0; i < xs->count - 1; i++) {
        if (xs->items[i + 1].t >= x.t) {
            home = i;
            break;
        }
    }

    // Shift everything at home or to its right one step to the right to make room for the new item.
    for (size_t i = xs->count; i > home; i--) {
        xs->items[i] = xs->items[i - 1];
    }

    xs->items[home] = x;
    xs->count += 1;
    return 0;
}

Ray ray_transform(Ray ray, Mat4D transform)
{
    Vec4D origin = mat4d_mul_vec4d(transform, ray.origin);
    Vec4D direction = mat4d_mul_vec4d(transform, ray.direction);
    return (Ray){ origin, direction };
}

Vec4D ray_position(Ray ray, double t)
{
    return d4_add(ray.origin, d4_mul(ray.direction, t));
}

IntersectionList ray_intersect_sphere(Ray ray, Sphere sphere)
{
    // Transform the ray into the sphere's object space
    Mat4D inv = mat4d_inverse(sphere.transform);
    Ray r = ray_transform(ray, inv);

    // Vector from sphere's centre to ray origin
    Vec4D sphere_to_ray = d4_sub(r.origin, d4_point(0., 0., 0.));

    double a = d4_dot(r.direction, r.direction);
    double b = 2 * d4_dot(r.direction, sphere_to_ray);
    double c = d4_dot(sphere_to_ray, sphere_to_ray) - 1;

    double discriminant = b * b - 4 * a * c;

    IntersectionList xs = intersection_list_new();
    if (discriminant < 0) {
        return xs;
    }

    double root = sqrt(discriminant);
    double t1 = (-b - root) / (2 * a);
    double t2 = (-b + root) / (2 * a);

    intersection_list_add(&xs, (Intersection){ t1, &sphere });
    intersection_list_add(&xs, (Intersection){ t2, &sphere });
    return xs;
}

IntersectionList ray_intersect_world(Ray ray, World world)
{
    IntersectionList xs = intersection_list_new();
    for (size_t i = 0; i < world.object_count; i++) {
        IntersectionList sub_xs = ray_intersect_sphere(ray, world.objects[i]);
        for (size_t j = 0; j < sub_xs.count; j++) {
            intersection_list_add(&xs, sub_xs.items[j]);
        }
    }
    return xs;
}

Intersection *hit(IntersectionList intersections) {
    double best = INFINITY;
    Intersection *best_ptr = NULL;
    for (size_t i = 0; i < intersections.count; i++) {
        Intersection *candidate = &intersections.items[i];
        if (candidate->t >= 0.0 && candidate->t < best) {
            best = candidate->t;
            best_ptr = candidate;
        }
    }
    return best_ptr;
}