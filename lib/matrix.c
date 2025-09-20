#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <matrix.h>

Mat4D mat4d_new(double vals[16]) {
    double *ptr = (double *)malloc(16 * sizeof(double));
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int idx = i * 4 + j;
            ptr[idx] = vals[idx];
        }
    }
    return ptr;
}

Mat4D mat4d_identity()
{
    double *ptr = (double *)malloc(16 * sizeof(double));
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int idx = i * 4 + j;
            double val = i == j ? 1.0 : 0.0;
            ptr[idx] = val;
        }
    }
    return ptr;
}

Mat4D mat4d_mul_mat4d(Mat4D a, Mat4D b)
{
    double *ptr = (double *)malloc(16 * sizeof(double));
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            double x = 0.0;
            for (int k = 0; k < 4; k++) {
                x += a[4*i + k] * b[4*k + j];
            }
            ptr[4*i + j] = x;
        }
    }
    return ptr;
}

Vec4D mat4d_mul_vec4d(Mat4D a, Vec4D b)
{
    Vec4D ret = { 0.0, 0.0, 0.0, 0.0 };
    return ret;
}

Mat4D mat4d_transpose(Mat4D a)
{
    // TODO
    return mat4d_identity();
}

Mat4D mat4d_inverse(Mat4D a)
{
    // TODO
    return mat4d_identity();
}

void mat4d_dbg(Mat4D a) {
    for (int iRow = 0; iRow < 4; iRow++) {
        int x = 4 * iRow;
        printf("| %6f  %6f  %6f  %6f |\n", a[x], a[x+1], a[x+2], a[x+3]);
    }
}

// Transformation matrices

Mat4D translation(double x, double y, double z) {
    return mat4d_new((double[]){ 
        1.0, 0.0, 0.0, x,
        0.0, 1.0, 0.0, y,
        0.0, 0.0, 0.0, z,
        0.0, 0.0, 0.0, 1.0
    });
}

Mat4D scaling(double x, double y, double z) {
    return mat4d_new((double[]){
        x, 0.0, 0.0, 0.0,
        0.0, y, 0.0, 0.0,
        0.0, 0.0, z, 0.0,
        0.0, 0.0, 0.0, 1.0
    });
}

Mat4D rotation_x(double theta) {
    return mat4d_new((double[]) {
        1.0, 0.0, 0.0, 0.0,
        0.0, cos(theta), -sin(theta), 0.0,
        0.0, sin(theta), cos(theta), 0.0,
        0.0, 0.0, 0.0, 1.0
    });
}

Mat4D rotation_y(double theta) {
    return mat4d_new((double[]) {
        cos(theta), 0.0, sin(theta), 0.0,
        0.0, 1.0, 0.0, 0.0,
        -sin(theta), 0.0, cos(theta), 0.0,
        0.0, 0.0, 0.0, 1.0
    });
}

Mat4D rotation_z(double theta) {
    return mat4d_new((double[]) {
        cos(theta), -sin(theta), 0.0, 0.,
        sin(theta), cos(theta), 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    });
}

Mat4D shearing(double xy, double xz, double yx, double yz, double zx, double zy)
{
    return mat4d_new((double[]) {
        1.0, xy, xz, 0.0,
        yx, 1.0, yz, 0.0,
        zx, zy, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    });
}
