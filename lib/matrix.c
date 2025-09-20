#include <matrix.h>
#include <stdlib.h>
#include <stdio.h>


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
