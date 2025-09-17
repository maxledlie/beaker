/* 4-dimensional vectors. See Chapter 1 of The Ray Tracer Challenge for the semantics. */

typedef struct Double4 {
    double x;
    double y;
    double z;
    double w;
} Double4;

typedef Double4 D4;

D4 d4_vector(double x, double y, double z);
D4 d4_point(double x, double y, double z);

int d4_is_vector(D4 v);
int d4_is_point(D4 v);

D4 d4_add(D4 a, D4 b);
D4 d4_sub(D4 a, D4 b);
D4 d4_mul(D4 a, double scale);
D4 d4_div(D4 a, double scale);
D4 d4_neg(D4 a);

double d4_mag(D4 a);
D4 d4_norm(D4 a);

double d4_dot(D4 a, D4 b);
D4 d4_cross(D4 a, D4 b);

// ----------------------------------------
