#include <matrix.h>
#include <stdlib.h>

Mat4D mat4d_identity()
{
    double m[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m[i][j] = 0.0;
        }
    }
    return m;
}

