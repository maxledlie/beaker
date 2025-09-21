#include <color.h>

Color color_add(Color a, Color b) {
    Color ret = {
        a.r + b.r,
        a.g + b.g,
        a.b + b.b
    };
    return ret;
}

Color color_sub(Color a, Color b) {
    Color ret = {
        a.r - b.r,
        a.g - b.g,
        a.b - b.b
    };
    return ret;
}

Color color_mul(Color a, double scale) {
    Color ret = {
        a.r * scale,
        a.g * scale,
        a.b * scale
    };
    return ret;
}

Color color_div(Color a, double scale) {
    Color ret = {
        a.r / scale,
        a.g / scale,
        a.b / scale
    };
    return ret;
}

Color color_hadamard(Color a, Color b) {
    Color ret = {
        a.r * b.r,
        a.g * b.g,
        a.b * b.b
    };
    return ret;
}

Color color_black()
{
    return (Color) {0.0, 0.0, 0.0 };
}
