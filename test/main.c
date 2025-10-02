#include <config.h>
#include <assertions.h>
#include <vector.h>
#include <ray.h>
#include <lighting.h>
#include <shape.h>

const double TOL = 0.0000000001;

// -------------------
// Helper functions
// -------------------

Shape glass_sphere(){
    Material glassy = material_default();
    glassy.transparency = 1;
    glassy.refractive_index = 1.5;
    return sphere_new(mat4d_identity(), glassy, "glass_sphere");
}

// -------------------
// Inverting Matrices
// -------------------

void test_mat4d_submatrix() {
    Mat4D a = mat4d_new((double []) {
        -6.,  1.,  1.,  6.,
        -8.,  5.,  8.,  6.,
        -1.,  0.,  8.,  2.,
        -7.,  1., -1.,  1.
    });

    Mat3D expected = mat3d_new((double[]) {
        -6.,  1.,  6.,
        -8.,  8.,  6.,
        -7., -1.,  1.
    });

    Mat3D result = mat4d_submatrix(a, 2, 1);
    assert_eq_mat3d(result, expected, TOL);
}

void test_mat4d_inverse() {
    Mat4D a = mat4d_new((double[]) {
        -5.,  2.,  6., -8.,
         1., -5.,  1.,  8.,
         7.,  7., -6., -7.,
         1., -3.,  7.,  4.
    });
    Mat4D b = mat4d_inverse(a);
    assert_eq_double(mat4d_determinant(a), 532., TOL);
    assert_eq_double(mat4d_cofactor(a, 2, 3), -160., TOL);
    assert_eq_double(b.m[3][2], -160. / 532., TOL);
    assert_eq_double(mat4d_cofactor(a, 3, 2), 105., TOL);
    assert_eq_double(b.m[2][3], 105. / 532., TOL);

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
    Shape sphere = sphere_default();
    IntersectionList xs = ray_intersect_shape(ray, &sphere);
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
    Shape sphere = sphere_new(translation(0.0, 1.0, 0.0), material_default(), "debug");
    Vec4D n = shape_normal(&sphere, d4_point(0.0, 1.70711, -0.70711));
    assert_eq_vec4d(n, d4_vector(0.0, 0.70711, -0.70711), 0.00001);
}

void test_hit__all_intersections_positive_t() {
    Shape sphere = sphere_default();
    IntersectionList xs = intersection_list_new();
    Intersection i1 = { 1.0, &sphere };
    Intersection i2 = { 2.0, &sphere };
    intersection_list_add(&xs, i2);
    intersection_list_add(&xs, i1);
    Intersection *i = hit(xs);
    assert_eq_double(i->t, 1.0, TOL);
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
    Material m = material_default();
    Vec4D position = d4_point(0., 0., 0.);
    Vec4D eyev = d4_vector(0., 0., -1.);
    Vec4D normalv = d4_vector(0., 0., -1.);
    PointLight light = (PointLight){ d4_point(0., 0., -10.), (Color) { 1., 1., 1. }};
    Shape obj = sphere_new(mat4d_identity(), m, "debug");

    Color result = lighting_compute(obj, light, position, eyev, normalv, 0);
    assert_eq_double(result.r, 1.9, TOL);
    assert_eq_double(result.g, 1.9, TOL);
    assert_eq_double(result.b, 1.9, TOL);
}

void test_lighting__eye_between_light_and_surface__eye_offset_45() {
    Material m = material_default();
    Vec4D position = d4_point(0., 0., 0.);
    Vec4D eyev = d4_vector(0., sqrt(2) / 2.0, -sqrt(2) / 2.0);
    Vec4D normalv = d4_vector(0., 0., -1.);
    PointLight light = (PointLight){ d4_point(0., 0., -10.), (Color) { 1., 1., 1. }};
    Shape obj = sphere_new(mat4d_identity(), m, "debug");

    Color result = lighting_compute(obj, light, position, eyev, normalv, 0);
    assert_eq_double(result.r, 1.0, TOL);
    assert_eq_double(result.g, 1.0, TOL);
    assert_eq_double(result.b, 1.0, TOL);
}

void test_lighting__eye_in_path_of_reflection_vector() {
    Material m = material_default();
    Vec4D position = d4_point(0., 0., 0.);
    Vec4D eyev = d4_vector(0., -sqrt(2) / 2.0, -sqrt(2) / 2.0);
    Vec4D normalv = d4_vector(0., 0., -1.);
    PointLight light = (PointLight){ d4_point(0., 10., -10.), (Color) { 1., 1., 1. }};
    Shape obj = sphere_new(mat4d_identity(), m, "debug");

    Color result = lighting_compute(obj, light, position, eyev, normalv, 0);
    assert_eq_double(result.r, 1.6364, 0.00001);
    assert_eq_double(result.g, 1.6364, 0.00001);
    assert_eq_double(result.b, 1.6364, 0.00001);
}

// ------------------------
// Making a Scene
// ------------------------

void test_computations(){
    Ray r = (Ray) {d4_point(0.,0.,-5.), d4_vector(0., 0., 1.)};
    Shape sphere = sphere_default();
    Intersection i = (Intersection) {4, &sphere};
    IntersectionData data = ray_prepare_computations(r, i);
    assert_eq_double(data.t, i.t, TOL);
    assert_eq_vec4d(data.point, d4_point(0., 0., -1.), TOL);
    assert_eq_vec4d(data.eyev, d4_vector(0., 0., -1.), TOL);
    assert_eq_vec4d(data.normalv, d4_vector(0., 0., -1.), TOL);
}

void test_ray_color__ray_misses() {
    World w = world_default();
    Ray r = (Ray) { d4_point(0., 0., -5.), d4_vector(0., 1., 0.) };
    Color c = ray_color(r, w, CFG_RECURSION_DEPTH);
    assert_eq_color(c, color_black(), TOL);
}

void test_ray_color__ray_hits() {
    World w = world_default();
    Ray r = (Ray) { d4_point(0., 0., -5.), d4_vector(0., 0., 1.) };
    Color c = ray_color(r, w, CFG_RECURSION_DEPTH);
    assert_eq_color(c, color_rgb(0.38066, 0.47583, 0.2855), 0.00001);
}

void test_ray_color__intersection_behind_ray() {
    World w = world_default();
    w.objects[0].material.ambient = 1.0;
    w.objects[1].material.ambient = 1.0;
    Ray r = (Ray) { d4_point(0., 0., 0.75), d4_vector(0., 0., -1.) };
    Color c = ray_color(r, w, CFG_RECURSION_DEPTH);
    assert_eq_color(c, w.objects[1].material.pattern.a, TOL);
}

int main() {
    test_mat4d_submatrix();
    test_mat4d_inverse();

    test_ray_intersect_sphere__sphere_behind_ray();
    test_ray_position();

    test_sphere_normal__translated();

    test_hit__all_intersections_positive_t();

    test_vec4d_reflect__approaching_at_45();
    test_lighting__eye_between_light_and_surface();
    test_lighting__eye_between_light_and_surface__eye_offset_45();
    test_lighting__eye_in_path_of_reflection_vector();

    test_ray_color__ray_misses();
    test_ray_color__ray_hits();
    test_ray_color__intersection_behind_ray();

    printf("Testing complete\n");
}