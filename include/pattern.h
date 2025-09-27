#pragma once

#include <matrix.h>
#include <color.h>

#define PATTERN_PLAIN 0
#define PATTERN_STRIPE 1
#define PATTERN_GRADIENT 2
#define PATTERN_RING 3
#define PATTERN_CHECKER 4

typedef struct {
    int type;
    Mat4D transform;
    Color a;
    Color b;
} Pattern;

Pattern pattern_plain_new(Color color);
Pattern pattern_stripe_new(Color a, Color b);
Pattern pattern_gradient_new(Color a, Color b);
Pattern pattern_ring_new(Color a, Color b);
Pattern pattern_checker_new(Color a, Color b);

Color pattern_color_at(Pattern pattern, Vec4D point);