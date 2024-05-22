#pragma once

#include "defines.h"
#include <cmath>

struct vec3 {
    float x;
    float y;
    float z;

    inline vec3() {}

    inline vec3(float value) {
        x = value;
        y = value;
        z = value;
    }

    inline vec3(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    inline bool operator==(const vec3& other) const {
        if (fabs(x - other.x) > MATH_FLOAT_EPSILON) {
            return false;
        }
        if (fabs(y - other.y) > MATH_FLOAT_EPSILON) {
            return false;
        }
        if (fabs(z - other.z) > MATH_FLOAT_EPSILON) {
            return false;
        }  
        return true;
    }

    inline vec3 operator+(const vec3& other) const {
        return vec3(x + other.x, y + other.y, z + other.z);
    }

    inline vec3 operator-(const vec3& other) const {
        return vec3(x - other.x, y - other.y, z - other.z);
    }

    inline vec3 operator*(const float scaler) const {
        return vec3(x * scaler, y * scaler, z * scaler);
    }

    inline vec3 operator/(const float scaler) const {
        return vec3(x / scaler, y / scaler, z / scaler);
    }

    inline vec3& operator+=(const vec3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    inline vec3& operator-=(const vec3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    inline vec3& operator*=(const float scaler) {
        x *= scaler;
        y *= scaler;
        z *= scaler;
        return *this;
    }

    inline vec3& operator/=(const float scaler) {
        x /= scaler;
        y /= scaler;
        z /= scaler;
        return *this;
    }

    inline float length() const {
        return sqrtf((x * x) + (y * y) + (z * z));
    }

    inline vec3 normalized() const {
        float _length = length();
        if (_length == 0.0f) {
            return vec3(0.0f);
        }
        return vec3(x / _length, y / _length, z / _length);
    }

    inline float distance_to(const vec3& other) const {
        return vec3(x - other.x, y - other.y, z - other.z).length();
    }

    inline vec3 direction_to(const vec3& other) const {
        return vec3(x - other.x, y - other.y, z - other.z).normalized();
    }

    inline static float dot(const vec3& a, const vec3& b) {
        return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
    }

    inline static vec3 cross(const vec3& a, const vec3& b) {
        return vec3((a.y * b.z) - (a.z * b.y),
                    (a.z * b.x) - (a.x * b.z),
                    (a.x * b.y) - (a.y * b.x));
    }

    inline static vec3 lerp(const vec3& a, const vec3& b, float time) {
        return a + ((b - a) * time);
    }
};

// vec3 constants
static const vec3 VEC3_ZERO = vec3(0.0f, 0.0f, 0.0f);
static const vec3 VEC3_UP = vec3(0.0f, -1.0f, 0.0f);
static const vec3 VEC3_DOWN = vec3(0.0f, 1.0f, 0.0f);
static const vec3 VEC3_LEFT = vec3(-1.0f, 0.0f, 0.0f);
static const vec3 VEC3_RIGHT = vec3(1.0f, 0.0f, 0.0f);
static const vec3 VEC3_FORWARD = vec3(0.0f, 0.0f, -1.0f);
static const vec3 VEC3_BACK = vec3(0.0f, 0.0f, 1.0f);