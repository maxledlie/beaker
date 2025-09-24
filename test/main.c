#include <assertions.h>
#include <vector.h>
#include <ray.h>
#include <lighting.h>

const double TOL = 0.0000000001;

void test_matrix_inverse() {
    Mat4D a = mat4d_new((double[]) {
        -5.,  2.,  6., -8.,
         1., -5.,  1.,  8.,
         7.,  7., -6., -7.,
         1., -3.,  7.,  4.
    });
    Mat4D b = mat4d_inverse(a);
    assert_eq_double(mat4d_determinant(a), 532., TOL);
    assert_eq_double(mat4d_cofactor(a, 2, 3), -160., TOL);
    assert_eq_double(b[3*4 + 2], -160. / 532., TOL);
    assert_eq_double(mat4d_cofactor(a, 3, 2), 105., TOL);
    assert_eq_double(b[2*4 + 3], 105. / 532., TOL);

    Mat4D expected = mat4d_new((double []) {
         0.21805,  0.45113,  0.24060, -0.04511,
        -0.80827, -1.45677, -0.44361,  0.52068,
        -0.07895, -0.22368, -0.05263,  0.19737,
        -0.52256, -0.81391, -0.30075,  0.30639
    });
    assert_eq_mat4d(b, expected, 0.00001);
}

// ----------------------------
// Ray-Sphere Intersections
// ----------------------------

/// A sphere is behind a ray
void test_ray_intersect_sphere__sphere_behind_ray() {
    Ray ray = { d4_point(0.0, 0.0, 5.0), d4_vector(0.0, 0.0, 1.0) };
    Sphere sphere = sphere_new();
    IntersectionList xs = ray_intersect_sphere(ray, sphere);
    assert_eq_int(xs.count, 2);
    assert_eq_double(xs.items[0].t, -6.0, TOL);
    assert_eq_double(xs.items[1].t, -4.0, TOL);
}

void test_ray_position() {
    Ray ray = { d4_point(2., 3., 4.), d4_vector(1., 0., 0.) };
    assert_eq_vec4d(ray_position(ray, 0.), d4_point(2., 3., 4.), TOL);
    assert_eq_vec4d(ray_position(ray, 1.), d4_point(3., 3., 4.), TOL);
    assert_eq_vec4d(ray_position(ray, -1.), d4_point(1., 3., 4.), TOL);
    assert_eq_vec4d(ray_position(ray, 2.5), d4_point(4.5, 3., 4.), TOL);
}

void test_sphere_normal__translated() {
    Sphere sphere = sphere_new();
    sphere.transform = translation(0.0, 1.0, 0.0);
    Vec4D n = sphere_normal(sphere, d4_point(0.0, 1.70711, -0.70711));
    assert_eq_vec4d(n, d4_vector(0.0, 0.70711, -0.70711), 0.00001);
}

void test_hit__all_intersections_positive_t() {
    Sphere sphere = sphere_new();
    IntersectionList xs = intersection_list_new();
    Intersection i1 = { 1.0, &sphere };
    Intersection i2 = { 2.0, &sphere };
    intersection_list_add(&xs, i1);
    intersection_list_add(&xs, i2);
    Intersection *i = hit(xs);
    assert_eq_ptr(i, &xs.items[1]);
}

/// --------------------------
/// The Phong Reflection Model
/// --------------------------

void test_vec4d_reflect__approaching_at_45() {
    Vec4D v = d4_vector(1., -1., 0.);
    Vec4D n = d4_vector(0., 1., 0.);
    Vec4D r = d4_reflect(v, n);
    assert_eq_vec4d(r, d4_vector(1., 1., 0.), TOL);
}

void test_lighting__eye_between_light_and_surface() {
    Material m = material_new();
    Vec4D position = d4_point(0., 0., 0.);
    Vec4D eyev = d4_vector(0., 0., -1.);
    Vec4D normalv = d4_vector(0., 0., -1.);
    PointLight light = (PointLight){ d4_point(0., 0., -10.), (Color) { 1., 1., 1. }};

    Color result = lighting_compute(m, light, position, eyev, normalv);
    assert_eq_double(result.r, 1.9, TOL);
    assert_eq_double(result.g, 1.9, TOL);
    assert_eq_double(result.b, 1.9, TOL);
}

void test_lighting__eye_between_light_and_surface__eye_offset_45() {
    Material m = material_new();
    Vec4D position = d4_point(0., 0., 0.);
    Vec4D eyev = d4_vector(0., sqrt(2) / 2.0, -sqrt(2) / 2.0);
    Vec4D normalv = d4_vector(0., 0., -1.);
    PointLight light = (PointLight){ d4_point(0., 0., -10.), (Color) { 1., 1., 1. }};

    Color result = lighting_compute(m, light, position, eyev, normalv);
    assert_eq_double(result.r, 1.0, TOL);
    assert_eq_double(result.g, 1.0, TOL);
    assert_eq_double(result.b, 1.0, TOL);
}

void test_lighting__eye_in_path_of_reflection_vector() {
    Material m = material_new();
    Vec4D position = d4_point(0., 0., 0.);
    Vec4D eyev = d4_vector(0., -sqrt(2) / 2.0, -sqrt(2) / 2.0);
    Vec4D normalv = d4_vector(0., 0., -1.);
    PointLight light = (PointLight){ d4_point(0., 10., -10.), (Color) { 1., 1., 1. }};

    Color result = lighting_compute(m, light, position, eyev, normalv);
    assert_eq_double(result.r, 1.6364, 0.00001);
    assert_eq_double(result.g, 1.6364, 0.00001);
    assert_eq_double(result.b, 1.6364, 0.00001);
}

// ------------------------
// Making a Scene
// ------------------------

/// Intersects a ray with the default world. Since there are two concentric spheres,
/// there should be four intersection points.
void test_ray_intersect_world__default_world() {
    World w = world_default();
    Ray r = (Ray) { d4_point(0., 0., -5.), d4_vector(0., 0., 1.) };
    IntersectionList xs = ray_intersect_world(r, w);
    assert_eq_size_t(xs.count, 4);
    assert_eq_double(xs.items[0].t, 4.0, TOL);
    assert_eq_double(xs.items[1].t, 4.5, TOL);
    assert_eq_double(xs.items[2].t, 5.5, TOL);
    assert_eq_double(xs.items[3].t, 6.0, TOL);
}

int main() {
    test_matrix_inverse();

    test_ray_intersect_sphere__sphere_behind_ray();
    test_ray_position();

    test_sphere_normal__translated();

    test_hit__all_intersections_positive_t();

    test_vec4d_reflect__approaching_at_45();
    test_lighting__eye_between_light_and_surface();
    test_lighting__eye_between_light_and_surface__eye_offset_45();
    test_lighting__eye_in_path_of_reflection_vector();

    test_ray_intersect_world__default_world();

    printf("Testing complete\n");
}