#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <matrix.h>

double mat2d_determinant(Mat2D a) {
    return a.m[0][0] * a.m[1][1] - a.m[0][1] * a.m[1][0];
}

Mat4D mat4d_new(double vals[16]) {
    Mat4D result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] = vals[4*i + j];
        }
    }
    return result;
}

Mat4D mat4d_identity()
{
    Mat4D result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] = (i == j) ? 1.0 : 0.0;
        }
    }
    return result;
}

Mat4D mat4d_mul_mat4d(Mat4D a, Mat4D b)
{
    Mat4D result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            double x = 0.0;
            for (int k = 0; k < 4; k++) {
                x += a.m[i][k] * b.m[k][j];
            }
            result.m[i][j] = x;
        }
    }
    return result;
}

Vec4D mat4d_mul_vec4d(Mat4D a, Vec4D b)
{
    Vec4D result;
    result.x = a.m[0][0] * b.x + a.m[0][1] * b.y + a.m[0][2] * b.z + a.m[0][3] * b.w;
    result.y = a.m[1][0] * b.x + a.m[1][1] * b.y + a.m[1][2] * b.z + a.m[1][3] * b.w;
    result.z = a.m[2][0] * b.x + a.m[2][1] * b.y + a.m[2][2] * b.z + a.m[2][3] * b.w;
    result.w = a.m[3][0] * b.x + a.m[3][1] * b.y + a.m[3][2] * b.z + a.m[3][3] * b.w;
    return result;
}

Mat4D mat4d_transpose(Mat4D a)
{
    Mat4D result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] = a.m[j][i];
        }
    }
    return result;
}

double mat4d_determinant(Mat4D a)
{
    double ret = 0.0;
    for (int jCol = 0; jCol < 4; jCol++) {
        ret += a.m[0][jCol] * mat4d_cofactor(a, 0, jCol);
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
    double vals[9];
    int write_index = 0;
    for (int k = 0; k < 16; k++) {
        int i = k / 4;
        int j = k % 4;
        if (i != iRow && j != jCol) {
            vals[write_index++] = a.m[i][j];
        }
    }

    Mat3D result;
    for (int k = 0; k < 9; k++) {
        int i = k / 3;
        int j = k % 3;
        result.m[i][j] = vals[k];
    }
    return result;
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
        printf("| %6f  %6f  %6f  %6f |\n", a.m[iRow][0], a.m[iRow][1], a.m[iRow][2], a.m[iRow][3]);
    }
}

Mat3D mat3d_new(double vals[9])
{
    Mat3D result;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result.m[i][j] = vals[3*i + j];
        }
    }
    return result;
}

Mat2D mat3d_submatrix(Mat3D a, int iRow, int jCol)
{
    double vals[4];
    int write_index = 0;
    for (int k = 0; k < 9; k++) {
        int i = k / 3;
        int j = k % 3;
        if (i != iRow && j != jCol) {
            vals[write_index++] = a.m[i][j];
        }
    }

    Mat2D result;
    for (int k = 0; k < 4; k++) {
        int i = k / 2;
        int j = k % 2;
        result.m[i][j] = vals[k];
    }
    return result;
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
        ret += a.m[0][jCol] * mat3d_cofactor(a, 0, jCol);
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
