#pragma once

#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "matrix.h"

#define MATH_PI 3.14159265358979323846f
#define MATH_DEG2RAD_MULTIPLIER MATH_PI / 180.0f
#define MATH_RAD2DEG_MULTIPLIER 180.0f / MATH_PI

inline float deg_to_rad(float degrees) {
    return degrees * MATH_DEG2RAD_MULTIPLIER;
}

inline float rad_to_deg(float radians) {
    return radians * MATH_RAD2DEG_MULTIPLIER;
}