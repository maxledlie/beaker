#pragma once

/* 4-dimensional vectors. See Chapter 1 of The Ray Tracer Challenge for the semantics. */

typedef struct Vec4D {
    double x;
    double y;
    double z;
    double w;
} Vec4D;

Vec4D d4_vector(double x, double y, double z);
Vec4D d4_point(double x, double y, double z);

int d4_is_vector(Vec4D v);
int d4_is_point(Vec4D v);

Vec4D d4_add(Vec4D a, Vec4D b);
Vec4D d4_sub(Vec4D a, Vec4D b);
Vec4D d4_mul(Vec4D a, double scale);
Vec4D d4_div(Vec4D a, double scale);
Vec4D d4_neg(Vec4D a);

double d4_mag(Vec4D a);
Vec4D d4_norm(Vec4D a);

double d4_dot(Vec4D a, Vec4D b);
Vec4D d4_cross(Vec4D a, Vec4D b);

Vec4D d4_reflect(Vec4D in, Vec4D normal);

// ----------------------------------------
