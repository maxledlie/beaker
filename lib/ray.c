#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <config.h>
#include <vector.h>
#include <ray.h>
#include <shape.h>

const size_t BASE_INTERSECTION_COUNT = 4;

/// Creates a new, dynamically sized, always-sorted, insert-only list of intersections
IntersectionList intersection_list_new() {
    Intersection *ptr = malloc(BASE_INTERSECTION_COUNT * sizeof(Intersection));
    return (IntersectionList) { 0, BASE_INTERSECTION_COUNT, ptr};
}

void intersection_list_free(IntersectionList *xs) {
    free(xs->items);
}

int intersection_list_add(IntersectionList *xs, Intersection x)
{
    // Allocate additional memory if required
    if (xs->count >= xs->capacity) {
        xs->capacity *= 2;
        xs->items = realloc(xs->items, xs->capacity * sizeof(Intersection));
        if (!xs->items) {
            fprintf(stderr, "Out of memory!\n");
            exit(1);
        }
    }

    // Find the appropriate position for the intersection: t-values must be increasing.
    // OPT: A binary search for the correct insertion point may be more efficient when array gets large.
    size_t home = xs->count;
    for (size_t i = 0; i < xs->count; i++) {
        if (xs->items[i].t >= x.t) {
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

Ray ray_at_pixel(Camera camera, int px, int py)
{
    // Offset from edge of canvas to pixel's center
    double xoffset = (px + 0.5) * camera.pixel_size;
    double yoffset = (py + 0.5) * camera.pixel_size;

    // Untransformed coordinates of the pixel in world space.
    double world_x = camera.half_width - xoffset;
    double world_y = camera.half_height - yoffset;

    // Using the camera matrix, transform the canvas point and the origin,
    // and then compute the ray's direction vector.
    Mat4D inv = camera.inv_transform;
    Vec4D pixel = mat4d_mul_vec4d(inv, d4_point(world_x, world_y, -1));
    Vec4D origin = mat4d_mul_vec4d(inv, d4_point(0., 0., 0.));
    Vec4D direction = d4_norm(d4_sub(pixel, origin));
    
    return (Ray){ origin, direction };
}

Ray ray_transform(Ray ray, Mat4D transform)
{
    Vec4D origin = mat4d_mul_vec4d(transform, ray.origin);
    Vec4D direction = mat4d_mul_vec4d(transform, ray.direction);
    return (Ray){ origin, direction };
}

Vec4D ray_position(Ray ray, double t)
{
    Vec4D delta = d4_mul(ray.direction, t);
    Vec4D result = d4_add(ray.origin, delta);
    return result;
}

double ray_intersect_sphere(Ray ray) {
    // Vector from sphere's centre to ray origin
    Vec4D sphere_to_ray = d4_sub(ray.origin, d4_point(0., 0., 0.));

    double a = d4_dot(ray.direction, ray.direction);
    double b = 2 * d4_dot(ray.direction, sphere_to_ray);
    double c = d4_dot(sphere_to_ray, sphere_to_ray) - 1;

    double discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return INFINITY;
    }

    double root = sqrt(discriminant);
    double t1 = (-b - root) / (2 * a);
    double t2 = (-b + root) / (2 * a);

    double tmin = fmin(t1, t2);
    double tmax = fmax(t1, t2);
    if (tmin >= 0.0) {
        return tmin;
    } else if (tmax >= 0.0) {
        return tmax;
    }
    return INFINITY;
}

double ray_intersect_plane(Ray ray) {
    if (fabs(ray.direction.y) < EPSILON) {
        return INFINITY;
    }

    double t = -ray.origin.y / ray.direction.y;
    return t >= 0.0 ? t : INFINITY;
}

/// @brief Computes the smaller and larger t-values at which a ray will intersect the -1 and +1
/// planes on a single axis, writing results to tmin and tmax.
/// @param origin 
/// @param direction 
/// @param tmin 
/// @param tmax 
void _cube_check_axis(double origin, double direction, double *tmin, double *tmax) {
    double tmin_numerator = (-1 - origin);
    double tmax_numerator = (1 - origin);

    if (fabs(direction) >= EPSILON) {
        *tmin = tmin_numerator / direction;
        *tmax = tmax_numerator / direction;
    } else {
        *tmin = tmin_numerator * INFINITY;
        *tmax = tmax_numerator * INFINITY;
    }
    if (*tmin > *tmax) {
        // swap
        double tmp = *tmin;
        *tmin = *tmax;
        *tmax = tmp;
    }
}

double ray_intersect_cube(Ray ray) {
    double xtmin, xtmax, ytmin, ytmax, ztmin, ztmax;
    _cube_check_axis(ray.origin.x, ray.direction.x, &xtmin, &xtmax);
    _cube_check_axis(ray.origin.y, ray.direction.y, &ytmin, &ytmax);
    _cube_check_axis(ray.origin.z, ray.direction.z, &ztmin, &ztmax);

    double tmin = fmax(fmax(xtmin, ytmin), ztmin);
    double tmax = fmin(fmin(xtmax, ytmax), ztmax);

    if (tmin > tmax) {
        return INFINITY;
    }

    if (tmin >= 0.0) {
        return tmin;
    } else if (tmax >= 0.0) {
        return tmax;
    }
    return INFINITY;
}

/// @brief Checks whether the intersection at `t` is within a radius of 1 from the y axis
int _check_cap(Ray ray, double t) {
    double x = ray.origin.x + t * ray.direction.x;
    double z = ray.origin.z + t * ray.direction.z;
    return (pow(x, 2.0) + pow(z, 2.0)) <= 1.0;
}

/// @brief Returns the smallest non-negative t-value where the ray intersects with the given
/// cylinder's end caps, or INFINITY if there is no such intersection.
double _ray_intersect_cylinder_cap(Ray ray, Shape *cylinder) {
    if (!cylinder->closed || fabs(ray.direction.y) < EPSILON) {
        return INFINITY;
    }

    // Check lower end cap by intersecting ray with plane at y = cyl.minimum
    double tlower = (cylinder->ymin - ray.origin.y) / ray.direction.y;
    if (tlower < 0.0 || !_check_cap(ray, tlower)) { 
        tlower = INFINITY;
    }

    // Check upper end cap by intersecting ray with plane at y = cyl.maximum
    double tupper = (cylinder->ymax - ray.origin.y) / ray.direction.y;
    if (tupper < 0.0 || !_check_cap(ray, tupper)) {
        tupper = INFINITY;
    }

    return fmin(tlower, tupper);
}

double _ray_intersect_cylinder_side(Ray ray, Shape *cylinder) {
    double a = pow(ray.direction.x, 2.0) + pow(ray.direction.z, 2.0);

    if (fabs(a) < EPSILON) {
        // Ray is parallel to the y axis
        return INFINITY;
    }

    double b = 2.0 * ray.origin.x * ray.direction.x + 2.0 * ray.origin.z * ray.direction.z;
    double c = pow(ray.origin.x, 2.0) + pow(ray.origin.z, 2.0) - 1.0;
    double disc = pow(b, 2.0) - 4.0 * a * c;

    if (disc < 0.0) {
        // Ray does not intersect the cylinder
        return INFINITY;
    }

    double t0 = (-b - sqrt(disc)) / (2.0 * a);
    double t1 = (-b + sqrt(disc)) / (2.0 * a);

    double tmin = fmin(t0, t1);
    double y0 = ray.origin.y + tmin * ray.direction.y;
    if (tmin >= 0.0 && y0 > cylinder->ymin && y0 < cylinder->ymax) {
        return tmin;
    }

    double tmax = fmax(t0, t1);
    double y1 = ray.origin.y + tmax * ray.direction.y;
    if (tmax >= 0.0 && y1 > cylinder->ymin && y1 < cylinder->ymax) {
        return tmax;
    }

    return INFINITY;
}

double ray_intersect_cylinder(Ray ray, Shape *cylinder) {
    double t_side = _ray_intersect_cylinder_side(ray, cylinder);
    double t_cap = _ray_intersect_cylinder_cap(ray, cylinder);
    return fmin(t_side, t_cap);
}

/// @brief Returns the smallest positive t-value at which the ray intersects the given shape.
/// If there are no such t-values, returns INFINITY.
double ray_intersect_shape(Ray ray, Shape *shape) {
    // Transform the ray into the shape's object space
    Mat4D inv = shape->inv_transform;
    Ray r = ray_transform(ray, inv);

    switch (shape->type) {
        case SHAPE_SPHERE:
            return ray_intersect_sphere(r);
        case SHAPE_PLANE:
            return ray_intersect_plane(r);
        case SHAPE_CUBE:
            return ray_intersect_cube(r);
        case SHAPE_CYLINDER:
            return ray_intersect_cylinder(r, shape);
        default:
            printf("Unrecognised shape %i", shape->type);
            return INFINITY;
    }
}

Intersection ray_intersect_world(Ray ray, World world)
{
    if (CFG_VERBOSE) {
        printf(
            "Casting ray from (%f, %f, %f) to (%f, %f, %f)\n",
            ray.origin.x, ray.origin.y, ray.origin.z,
            ray.direction.x, ray.direction.y, ray.direction.z
        );
    }

    Intersection best = (Intersection) { INFINITY, NULL };
    for (size_t i = 0; i < world.object_count; i++) {
        double t = ray_intersect_shape(ray, &world.objects[i]);
        assert(t >= 0.0);
        if (t < best.t) {
            best = (Intersection) { t, &world.objects[i] };
        }
    }
    return best;

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
    if (CFG_VERBOSE) {
        if (best_ptr) {
            printf("Hit at object %s\n", best_ptr->object_ptr->name);
            printf("Hit at t-value %f\n", best_ptr->t);
        } else {
            printf("No hit.\n");
        }
    }
    return best_ptr;
}

IntersectionData ray_prepare_computations(Ray r, Intersection i)
{
    IntersectionData d;
    d.t = i.t;
    d.object_ptr = i.object_ptr;
    d.point = ray_position(r, d.t);
    d.eyev = d4_neg(r.direction);
    d.normalv = shape_normal(d.object_ptr, d.point);
    d.over_point = d4_add(d.point, d4_mul(d.normalv, EPSILON));
    d.reflectv = d4_reflect(r.direction, d.normalv);

    // Handle case where eye is *inside* the object, so normal vector points away
    if (d4_dot(d.normalv, d.eyev) < 0.0) {
        d.normalv = d4_neg(d.normalv);
        d.inside = 1;
    } else {
        d.inside = 0;
    }
    return d;
}

Color reflected_color(World w, IntersectionData x, int remaining_reflections) {
    if (remaining_reflections <= 0) {
        return color_black();
    }
    double reflective = x.object_ptr->material.reflective;
    if (reflective == 0.0) {
        return color_black();
    }
    Ray reflected_ray = (Ray) { x.over_point, x.reflectv };
    Color c = ray_color(reflected_ray, w, remaining_reflections - 1);
    return color_mul(c, reflective);
}

Color shade_hit(World world, IntersectionData data, int remaining_reflections) {
    Color c = color_black();
    for (size_t i = 0; i < world.light_count; i++) {
        PointLight light = world.lights[i];
        int in_shadow = is_point_shadowed(data.over_point, light, world);

        if (CFG_VERBOSE) {
            if (in_shadow) {
                printf("Intersection is in shadow\n");
            } else {
                printf("Intersection is NOT in shadow\n");
            }
        }

        Color contribution = lighting_compute(
            *data.object_ptr,
            light,
            data.point,
            data.eyev,
            data.normalv,
            in_shadow
        );

        Color reflection = reflected_color(world, data, remaining_reflections);        
        c = color_add(c, contribution);
        c = color_add(c, reflection);
    }
    return c;
}

Color ray_color(Ray ray, World world, int remaining_reflections) {
    Intersection h = ray_intersect_world(ray, world);
    if (h.t == INFINITY) {
        return color_black();
    }

    IntersectionData data = ray_prepare_computations(ray, h);

    if (CFG_SINGLE_PIXEL_DEBUG) {
        printf("\n");
        printf("Intersection with object '%s' at t-value %f\n", data.object_ptr->name, data.t);
        printf("Intersection point: (%f, %f, %f)\n", data.point.x, data.point.y, data.point.z);
        printf("Intersection over_point: (%f, %f, %f)\n", data.over_point.x, data.over_point.y, data.over_point.z);
    }

    Color c = shade_hit(world, data, remaining_reflections);
    return c;
}
