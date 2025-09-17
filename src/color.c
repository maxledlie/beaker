#include <color.h>

Color d3_add(Color a, Color b) {
    Color ret = {
        a.r + b.r,
        a.g + b.g,
        a.b + b.b
    };
    return ret;
}

Color d3_sub(Color a, Color b) {
    Color ret = {
        a.r - b.r,
        a.g - b.g,
        a.b - b.b
    };
    return ret;
}

Color d3_mul(Color a, double scale) {
    Color ret = {
        a.r * scale,
        a.g * scale,
        a.b * scale
    };
    return ret;
}

Color d3_div(Color a, double scale) {
    Color ret = {
        a.r / scale,
        a.g / scale,
        a.b / scale
    };
    return ret;
}

Color d3_hadamard(Color a, Color b) {
    Color ret = {
        a.r * b.r,
        a.g * b.g,
        a.b * b.b
    };
    return ret;
}