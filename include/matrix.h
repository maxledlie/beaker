#pragma once

/* See Chapter 3 of the Ray Tracer Challenge */

#include <vector.h>

typedef double* Mat4D;
typedef double* Mat3D;
typedef double* Mat2D;

// 4D matrices
Mat4D mat4d_new(double vals[16]);
Mat4D mat4d_identity();
Mat4D mat4d_mul_mat4d(Mat4D a, Mat4D b);
Vec4D mat4d_mul_vec4d(Mat4D a, Vec4D b);
Mat4D mat4d_transpose(Mat4D a);
Mat4D mat4d_inverse(Mat4D a);
double mat4d_determinant(Mat4D a);
Mat3D mat4d_submatrix(Mat4D a, int x, int y);
double mat4d_cofactor(Mat4D a);
void mat4d_dbg(Mat4D a);

// 3D matrices
Mat2D mat3d_submatrix(Mat3D a, int x, int y);
double mat3d_minor(Mat3D a);
double mat3d_cofactor(Mat3D a);


// Transformation matrices
Mat4D translation(double x, double y, double z);
Mat4D scaling(double x, double y, double z);
Mat4D rotation_x(double theta);
Mat4D rotation_y(double theta);
Mat4D rotation_z(double theta);
Mat4D shearing(double xy, double xz, double yx, double yz, double zx, double zy);