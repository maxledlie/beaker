#include <math.h>
#include <pattern.h>
#include <stdio.h>

int int_floor(double x) {
    if (x >= 0.0) {
        return (int)x;
    }
    return (int)(x - 0.5);
}

Pattern _pattern_new(int type, Color a, Color b, Mat4D transform) {
    Mat4D inv = mat4d_inverse(transform);
    return (Pattern) { type, transform, inv, a, b };
}

Pattern pattern_plain_new(Color color, Mat4D transform) {
    return _pattern_new(PATTERN_PLAIN, color, color, transform);
}

Pattern pattern_stripe_new(Color a, Color b, Mat4D transform)
{
    return _pattern_new(PATTERN_STRIPE, a, b, transform);
}

Pattern pattern_gradient_new(Color a, Color b, Mat4D transform)
{
    return _pattern_new(PATTERN_GRADIENT, a, b, transform);
}

Pattern pattern_ring_new(Color a, Color b, Mat4D transform)
{
    return _pattern_new(PATTERN_RING, a, b, transform);
}

Pattern pattern_checker_new(Color a, Color b, Mat4D transform)
{
    return _pattern_new(PATTERN_CHECKER, a, b, transform);
}

Color _color_at_stripe(Pattern pattern, Vec4D point) {
    return (int)floor(point.x) % 2 ? pattern.a : pattern.b;
}

Color _color_at_gradient(Pattern pattern, Vec4D point) {
    Color a = color_mul(pattern.a, point.x);
    Color b = color_mul(pattern.b, 1. - point.x);
    return color_add(a, b);
}

Color _color_at_checker(Pattern pattern, Vec4D point) {
    int xflr = int_floor(point.x);
    int yflr = int_floor(point.y);
    int zflr = int_floor(point.z);
    return (xflr + yflr + zflr) % 2 ? pattern.a : pattern.b;
}

Color _color_at_ring(Pattern pattern, Vec4D point) {
    double r = sqrt(point.x * point.x + point.z + point.z);
    return (int) floor(r) % 2 ? pattern.a : pattern.b;
}

Color pattern_color_at(Pattern pattern, Vec4D point)
{
    switch (pattern.type) {
        case PATTERN_PLAIN:
            return pattern.a;
        case PATTERN_STRIPE:
            return _color_at_stripe(pattern, point);
        case PATTERN_GRADIENT:
            return _color_at_gradient(pattern, point);
        case PATTERN_CHECKER:
            return _color_at_checker(pattern, point);
        case PATTERN_RING:
            return _color_at_ring(pattern, point);
    }

    printf("Unknown pattern %i", pattern.type);
    return color_black();
}