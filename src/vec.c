#include <vec.h>

#include <math.h>

int d4_is_vector(D4 v) {
    return v.w == 0;
}

int d4_is_point(D4 v) {
    return v.w == 1;
}

D4 d4_vector(double x, double y, double z) {
    D4 ret = { x, y, z, 0.0 };
    return ret;
}

D4 d4_point(double x, double y, double z) {
    D4 ret = { x, y, z, 1.0 };
    return ret;
}

D4 d4_add(D4 a, D4 b) {
    D4 ret = {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z,
        a.w + b.w
    };
    return ret;
}

D4 d4_sub(D4 a, D4 b) {
    D4 ret = {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z,
        a.w - b.w
    };
    return ret;
}

D4 d4_cross(D4 a, D4 b) {
    D4 ret = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
        0.0
    };
    return ret;
}

D4 d4_neg(D4 a) {
    D4 ret = {
        -a.x,
        -a.y,
        -a.z,
        -a.z
    };
    return ret;
}

D4 d4_norm(D4 a) {
    return d4_div(a, d4_mag(a));
}

D4 d4_mul(D4 a, double scale) {
    D4 ret = {
        a.x * scale,
        a.y * scale,
        a.z * scale,
        a.w * scale
    };
    return ret;
}

D4 d4_div(D4 a, double scale) {
    D4 ret = {
        a.x / scale,
        a.y / scale,
        a.z / scale,
        a.w / scale
    };
    return ret;
}

double d4_mag(D4 a) {
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z + a.w  * a.w);
}

double d4_dot(D4 a, D4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}
