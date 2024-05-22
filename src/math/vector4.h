#pragma once

#include "defines.h"
#include <cmath>
#include <cstdint>

union vec4 {
    float elements[4];
    struct {
        float x;
        float y;
        float z;
        float w;
    };

    inline vec4() {}

    inline vec4(float value) {
        x = value;
        y = value;
        z = value;
        w = value;
    }

    inline vec4(float x, float y, float z, float w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    inline float& operator[](uint32_t index) {
        return elements[index];
    }

    inline const float& operator[](uint32_t index) const {
        return elements[index];
    }

    inline bool operator==(const vec4& other) const {
        if (fabs(x - other.x) > MATH_FLOAT_EPSILON) {
            return false;
        }
        if (fabs(y - other.y) > MATH_FLOAT_EPSILON) {
            return false;
        }
        if (fabs(z - other.z) > MATH_FLOAT_EPSILON) {
            return false;
        }  
        if (fabs(w - other.w) > MATH_FLOAT_EPSILON) {
            return false;
        }  
        return true;
    }

    inline vec4 operator+(const vec4& other) const {
        return vec4(x + other.x, y + other.y, z + other.z, w + other.w);
    }

    inline vec4 operator-(const vec4& other) const {
        return vec4(x - other.x, y - other.y, z - other.z, w - other.w);
    }

    inline vec4 operator*(const float scaler) const {
        return vec4(x * scaler, y * scaler, z * scaler, w * scaler);
    }

    inline vec4 operator/(const float scaler) const {
        return vec4(x / scaler, y / scaler, z / scaler, w / scaler);
    }

    inline vec4& operator+=(const vec4& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }

    inline vec4& operator-=(const vec4& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }

    inline vec4& operator*=(const float scaler) {
        x *= scaler;
        y *= scaler;
        z *= scaler;
        w *= scaler;
        return *this;
    }

    inline vec4& operator/=(const float scaler) {
        x /= scaler;
        y /= scaler;
        z /= scaler;
        w /= scaler;
        return *this;
    }

    inline float length() const {
        return sqrtf((x * x) + (y * y) + (z * z) + (w * w));
    }

    inline vec4 normalized() const {
        float _length = length();
        if (_length == 0.0f) {
            return vec4(0.0f);
        }
        return vec4(x / _length, y / _length, z / _length, w / _length);
    }
};