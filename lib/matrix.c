#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <matrix.h>

double mat2d_determinant(Mat2D a) {
    return a[0] * a[3] - a[1] * a[2];
}

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
    ret.x = a[0] * b.x + a[1] * b.y + a[2] * b.z + a[3] * b.w;
    ret.y = a[4] * b.x + a[5] * b.y + a[6] * b.z + a[7] * b.w;
    ret.z = a[8] * b.x + a[9] * b.y + a[10] * b.z + a[11] * b.w;
    ret.w = a[12] * b.x + a[13] * b.y + a[14] * b.z + a[15] * b.w;
    return ret;
}

Mat4D mat4d_transpose(Mat4D a)
{
    double *ptr = (double *)malloc(16 * sizeof(double));
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            ptr[4*i + j] = a[4*j + i];
        }
    }
    return ptr;
}

double mat4d_determinant(Mat4D a)
{
    double ret = 0.0;
    for (int jCol = 0; jCol < 4; jCol++) {
        ret += a[jCol] * mat4d_cofactor(a, 0, jCol);
    }
    return ret;
}

Mat4D mat4d_inverse(Mat4D a)
{
    double det = mat4d_determinant(a);
    assert(det != 0.0);

    double m[16];
    for (int iRow = 0; iRow < 4; iRow++) {
        for (int jCol = 0; jCol < 4; jCol++) {
            double c = mat4d_cofactor(a, iRow, jCol);
            m[4*jCol + iRow] = c / det;
        }
    }

    return mat4d_new(m);
}

Mat3D mat4d_submatrix(Mat4D a, int iRow, int jCol) {
    double *ptr = (double *)malloc(9 * sizeof(double));
    int write_index = 0;
    for (int k = 0; k < 16; k++) {
        int i = k / 4;
        int j = k % 4;
        if (i != iRow && j != jCol) {
            ptr[write_index++] = a[k];
        }
    }
    return ptr;

}

double mat4d_minor(Mat4D a, int iRow, int jCol) {
    return mat3d_determinant(mat4d_submatrix(a, iRow, jCol));
}

double mat4d_cofactor(Mat4D a, int iRow, int jCol) {
    double sign = (iRow + jCol) % 2 ? 1 : -1;
    return sign * mat4d_minor(a, iRow, jCol);
}


void mat4d_dbg(Mat4D a) {
    for (int iRow = 0; iRow < 4; iRow++) {
        int x = 4 * iRow;
        printf("| %6f  %6f  %6f  %6f |\n", a[x], a[x+1], a[x+2], a[x+3]);
    }
}

Mat2D mat3d_submatrix(Mat3D a, int iRow, int jCol) {
    double *ptr = (double *)malloc(4 * sizeof(double));
    int write_index = 0;
    for (int k = 0; k < 9; k++) {
        int i = k / 3;
        int j = k % 3;
        if (i != iRow && j != jCol) {
            ptr[write_index++] = a[k];
        }
    }
    return ptr;
}

double mat3d_minor(Mat3D a, int iRow, int jCol) {
    return mat2d_determinant(mat3d_submatrix(a, iRow, jCol));
}

double mat3d_cofactor(Mat3D a, int iRow, int jCol) {
    double sign = (iRow + jCol) % 2 ? 1 : -1;
    return sign * mat3d_minor(a, iRow, jCol);
}

double mat3d_determinant(Mat3D a) {
    double ret = 0.0;
    for (int jCol = 0; jCol < 3; jCol++) {
        ret += a[jCol] * mat3d_cofactor(a, 0, jCol);
    }
    return ret;
}

// Transformation matrices

Mat4D translation(double x, double y, double z) {
    return mat4d_new((double[]){ 
        1.0, 0.0, 0.0, x,
        0.0, 1.0, 0.0, y,
        0.0, 0.0, 1.0, z,
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
