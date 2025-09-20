#include <matrix.h>
#include <geometry.h>

Sphere sphere_new() {
    Mat4D transform = mat4d_identity();
    return (Sphere) { transform };
}