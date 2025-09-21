#pragma once

#include <color.h>

typedef struct Material {
    Color color;
    double ambient;
    double diffuse;
    double specular;
    double shininess;
} Material;

Material material_new();
