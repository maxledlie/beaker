#include <vector.h>

#include <math.h>

int d4_is_vector(Vec4D v) {
    return v.w == 0;
}

int d4_is_point(Vec4D v) {
    return v.w == 1;
}

Vec4D d4_vector(double x, double y, double z) {
    Vec4D ret = { x, y, z, 0.0 };
    return ret;
}

Vec4D d4_point(double x, double y, double z) {
    Vec4D ret = { x, y, z, 1.0 };
    return ret;
}

Vec4D d4_add(Vec4D a, Vec4D b) {
    Vec4D ret = {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z,
        a.w + b.w
    };
    return ret;
}

Vec4D d4_sub(Vec4D a, Vec4D b) {
    Vec4D ret = {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z,
        a.w - b.w
    };
    return ret;
}

Vec4D d4_cross(Vec4D a, Vec4D b) {
    Vec4D ret = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
        0.0
    };
    return ret;
}

Vec4D d4_neg(Vec4D a) {
    Vec4D ret = {
        -a.x,
        -a.y,
        -a.z,
        -a.z
    };
    return ret;
}

Vec4D d4_norm(Vec4D a) {
    return d4_div(a, d4_mag(a));
}

Vec4D d4_mul(Vec4D a, double scale) {
    Vec4D ret = {
        a.x * scale,
        a.y * scale,
        a.z * scale,
        a.w * scale
    };
    return ret;
}

Vec4D d4_div(Vec4D a, double scale) {
    Vec4D ret = {
        a.x / scale,
        a.y / scale,
        a.z / scale,
        a.w / scale
    };
    return ret;
}

double d4_mag(Vec4D a) {
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w);
}

double d4_dot(Vec4D a, Vec4D b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}
