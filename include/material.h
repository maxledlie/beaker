#pragma once

#include <color.h>
#include <pattern.h>

typedef struct Material {
    Pattern pattern;
    double ambient;
    double diffuse;
    double specular;
    double shininess;
} Material;

Material material_new();
