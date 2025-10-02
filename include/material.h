#pragma once

#include <color.h>
#include <pattern.h>

typedef struct Material {
    Pattern pattern;
    double ambient;
    double diffuse;
    double specular;
    double shininess;
    double reflective;
    double transparency;
    double refractive_index;
} Material;

Material material_default();
