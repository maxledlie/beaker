#include <stdlib.h>
#include <math.h>

#include <vector.h>
#include <ray.h>

Ray ray_transform(Ray ray, Mat4D transform) {
    Vec4D origin = mat4d_mul_vec4d(transform, ray.origin);
    Vec4D direction = mat4d_mul_vec4d(transform, ray.direction);
    return (Ray){ origin, direction };
}

IntersectionArray ray_intersect_sphere(Ray ray) {
    // Vector from sphere's centre to ray origin
    Vec4D sphere_to_ray = d4_sub(ray.origin, d4_point(0., 0., 0.));

    double a = d4_dot(ray.direction, ray.direction);
    double b = 2 * d4_dot(ray.direction, sphere_to_ray);
    double c = d4_dot(sphere_to_ray, sphere_to_ray) - 1;

    double discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return (IntersectionArray){ 0, NULL };
    }

    double root = sqrt(discriminant);
    double t1 = (-b - root) / (2 * a);
    double t2 = (-b + root) / (2 * a);

    // It feels silly to malloc two doubles. But for other geometries there may be many more intersections.
    Intersection *out = malloc(2 * sizeof(Intersection));
    Intersection i1 = { t1 };
    Intersection i2 = { t2 };
    out[0] = i1;
    out[1] = i2;
    return (IntersectionArray){ 2, out };
}

Intersection *hit(IntersectionArray intersections) {
    double best = INFINITY;
    int best_i = -1;
    for (size_t i = 1; i < intersections.count; i++) {
        Intersection intersection = intersections.items[i];
        if (intersection.t >= 0.0 && intersection.t < best) {
            best = intersection.t;
            best_i = i;
        }
    }
    return best_i >= 0 ? &intersections.items[best_i] : (Intersection *)NULL;
}