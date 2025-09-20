#include <assert.h>
#include <vector.h>
#include <ray.h>

/// A sphere is behind a ray
void test_ray_intersect_sphere__sphere_behind_ray() {
    Ray ray = { d4_point(0.0, 0.0, 5.0), d4_vector(0.0, 0.0, 1.0) };
    IntersectionArray xs = ray_intersect_sphere(ray);
    assert(xs.count == 2);
    assert(xs.items[0].t == -6.0);
    assert(xs.items[1].t == -4.0);
}

int main() {
    test_ray_intersect_sphere__sphere_behind_ray();
}