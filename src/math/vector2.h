#pragma once

#include "defines.h"
#include <cmath>

struct ivec2 {
    int x;
    int y;

    inline ivec2() {}

    inline ivec2(int x, int y) {
        this->x = x;
        this->y = y;
    }

    inline ivec2 operator+(const ivec2& other) {
        return ivec2(x + other.x, y + other.y);
    }

    inline ivec2 operator-(const ivec2& other) {
        return ivec2(x - other.x, y + other.y);
    }
};

struct vec2 {
    float x;
    float y;

    inline vec2() {}

    inline vec2(float x, float y) {
        this->x = x;
        this->y = y;
    }

    inline bool operator==(const vec2& other) const {
        if (fabs(x - other.x) > MATH_FLOAT_EPSILON) {
            return false;
        }
        if (fabs(y - other.y) > MATH_FLOAT_EPSILON) {
            return false;
        }
        return true;
    }
    
    inline vec2 operator+(const vec2& other) const {
        return vec2(x + other.x, y + other.y);
    }

    inline vec2 operator-(const vec2& other) const {
        return vec2(x - other.x, y + other.y);
    }

    inline vec2 operator*(const float other) const {
        return vec2(x * other, y * other);
    }

    inline vec2 operator/(const float other) const {
        return vec2(x / other, y / other);
    }

    inline vec2& operator+=(const vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    inline vec2& operator-=(const vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    inline vec2& operator*=(const float scaler) {
        x *= scaler;
        y *= scaler;
        return *this;
    }

    inline vec2& operator/=(const float scaler) {
        x /= scaler;
        y /= scaler;
        return *this;
    }

    inline float length() const {
        return sqrtf((x * x) + (y * y));
    }

    inline vec2 normalized() const {
        float _length = length();
        if (_length == 0.0f) {
            return vec2(0.0f, 0.0f);
        }
        return vec2(x / _length, y / _length);
    }

    inline float distance_to(const vec2& other) const {
        return vec2(x - other.x, y - other.y).length();
    }

    inline vec2 direction_to(const vec2& other) const {
        return vec2(x - other.x, y - other.y).normalized();
    }
};