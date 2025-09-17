#pragma once

/* RGB color operations. See Chapter 2 of the Ray Tracer Challenge. */

typedef struct Color {
    double r;
    double g;
    double b;
} Color;

Color color_add(Color a, Color b);
Color color_sub(Color a, Color b);
Color color_mul(Color a, double scale);
Color color_div(Color a, double scale);
Color color_hadamard(Color a, Color b);