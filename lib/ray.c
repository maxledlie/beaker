#include <stdlib.h>
#include <math.h>

#include <vector.h>
#include <ray.h>
#include <geometry.h>

Ray ray_transform(Ray ray, Mat4D transform) {
    Vec4D origin = mat4d_mul_vec4d(transform, ray.origin);
    Vec4D direction = mat4d_mul_vec4d(transform, ray.direction);
    return (Ray){ origin, direction };
}

Vec4D ray_position(Ray ray, double t)
{
    return d4_add(ray.origin, d4_mul(ray.direction, t));
}

IntersectionList ray_intersect_sphere(Ray ray, Sphere sphere) {
    // Transform the ray into the sphere's object space
    Mat4D inv = mat4d_inverse(sphere.transform);
    Ray r = ray_transform(ray, inv);

    // Vector from sphere's centre to ray origin
    Vec4D sphere_to_ray = d4_sub(r.origin, d4_point(0., 0., 0.));

    double a = d4_dot(r.direction, r.direction);
    double b = 2 * d4_dot(r.direction, sphere_to_ray);
    double c = d4_dot(sphere_to_ray, sphere_to_ray) - 1;

    double discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return (IntersectionList){ 0, NULL };
    }

    double root = sqrt(discriminant);
    double t1 = (-b - root) / (2 * a);
    double t2 = (-b + root) / (2 * a);

    // It feels silly to malloc two doubles. But for other geometries there may be many more intersections.
    Intersection *out = malloc(2 * sizeof(Intersection));
    Intersection i1 = { t1, &sphere };
    Intersection i2 = { t2, &sphere };
    out[0] = i1;
    out[1] = i2;
    return (IntersectionList){ 2, out };
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