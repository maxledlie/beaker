#pragma once

#include <stdio.h>
#include <math.h>
#include <assert.h>

#include <vector.h>
#include <matrix.h>
#include <color.h>

int assert_eq_double(double actual, double expected, double tol) {
    if (tol == 0) {
        if (actual != expected) {
            printf("Expected %f but was %f\n", expected, actual);
            return 0;
        }
    } else {
        double delta = actual - expected;
        if (fabs(delta) > tol) {
            printf("Expected %f but was %f. Delta of %f exceeds tolerance of %f\n", expected, actual, delta, tol);
            return 0;
        }
    }

    return 1;
}

int assert_eq_int(int actual, int expected) {
    if (actual != expected) {
        printf("Expected %d but was %d\n", expected, actual);
        return 0;
    }

    return 1;
}

int assert_eq_size_t(size_t actual, size_t expected) {
    if (actual != expected) {
        printf("Expected %zu but was %zu\n", expected, actual);
        return 0;
    }

    return 1;
}

int assert_eq_ptr(void *actual, void *expected) {
    if (actual != expected) {
        printf("Expected pointer to address %p but was %p\n", expected, actual);
        return 0;
    }
    return 1;
}

int assert_eq_vec4d(Vec4D actual, Vec4D expected, double tol) {
    return assert_eq_double(actual.x, expected.x, tol) &&
        assert_eq_double(actual.y, expected.y, tol) &&
        assert_eq_double(actual.z, expected.z, tol) &&
        assert_eq_double(actual.w, expected.w, tol);
}

int assert_eq_color(Color actual, Color expected, double tol) {
    return assert_eq_double(actual.r, expected.r, tol) &&
        assert_eq_double(actual.g, expected.g, tol) &&
        assert_eq_double(actual.b, expected.b, tol);
}

int assert_eq_mat4d(Mat4D actual, Mat4D expected, double tol) {
    int pass = 1;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            pass = pass && assert_eq_double(actual.m[i][j], expected.m[i][j], tol);
        }
    }
    return pass;
}

int assert_eq_mat3d(Mat3D actual, Mat3D expected, double tol) {
    int pass = 1;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            pass = pass && assert_eq_double(actual.m[i][j], expected.m[i][j], tol);
        }
    }
    return pass;
}