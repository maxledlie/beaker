#include <material.h>

Material material_new() {
    // Default shiny white material
    Color color = { 1.0, 1.0, 1.0 };
    Material ret ={
        color,
        0.1,
        0.9,
        0.9,
        200.0
    };
    return ret;
}