#include <assertions.h>
#include <vector.h>
#include <ray.h>

/// A sphere is behind a ray
void test_ray_intersect_sphere__sphere_behind_ray() {
    Ray ray = { d4_point(0.0, 0.0, 5.0), d4_vector(0.0, 0.0, 1.0) };
    Sphere sphere = sphere_new();
    IntersectionArray xs = ray_intersect_sphere(ray, sphere);
    assert_eq_int(xs.count, 2);
    assert_eq_double(xs.items[0].t, -6.0);
    assert_eq_double(xs.items[1].t, -4.0);
}

int main() {
    test_ray_intersect_sphere__sphere_behind_ray();
}