#pragma once

#include "defines.h"
#include "matrix.h"
#include "vector3.h"
#include <cmath>

struct quat {
    float x;
    float y;
    float z;
    float w;

    quat (float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    inline float normal() const {
        return sqrtf((x * x) + (y * y) + (z * z) + (w * w));
    }

    inline quat normalized() const {
        float _normal = normal();
        return quat(x / _normal, y / _normal, z / _normal, w / _normal);
    }

    inline quat conjugate() const {
        return quat(-x, -y, -z, w);
    }

    inline quat inverse() const {
        return conjugate().normalized();
    }

    inline bool operator==(const quat& other) {
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

    inline quat operator+(const quat& other) const {
        return quat(x + other.x, y + other.y, z + other.z, w + other.w);
    }

    inline quat operator-(const quat& other) const {
        return quat(x - other.x, y - other.y, z - other.z, w - other.w);
    }

    inline quat operator*(const quat& other) const {
        quat result;

        result.x = x * other.w +
                    y * other.z -
                    z * other.y +
                    w * other.x;

        result.y = -x * other.z +
                    y * other.w +
                    z * other.x +
                    w * other.y;

        result.z = x * other.y - 
                    y * other.x +
                    z * other.w +
                       w * other.z;

        result.w = -x * other.x - 
                    y * other.y -
                    z * other.z +
                    w * other.w;

        return result;
    }

    inline static float dot(const quat& a, const quat& b) {
        return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
    }

    inline mat4 to_mat4() const {
        mat4 result(1.0f);

        quat n = quat(x, y, z, w);

        float nxx = n.x * n.x;
        float nyy = n.y * n.y;
        float nzz = n.z * n.z;
        float nxz = n.x * n.z;
        float nxy = n.x * n.y;
        float nyz = n.y * n.z;
        float nwx = n.w * n.x;
        float nwy = n.w * n.y;
        float nwz = n.w * n.z;

        result[0][0] = 1.0f - 2.0f * (nyy + nzz);
        result[0][1] = 2.0f * (nxy + nwz);
        result[0][2] = 2.0f * (nxz - nwy);

        result[1][0] = 2.0f * (nxy - nwz);
        result[1][1] = 1.0f - 2.0f * (nxx + nzz);
        result[1][2] = 2.0f * (nyz + nwx);

        result[2][0] = 2.0f * (nxz + nwy);
        result[2][1] = 2.0f * (nyz - nwx);
        result[2][2] = 1.0f - 2.0f * (nxx + nyy);

        return result;
    }

    inline mat4 to_mat4_around_center(vec3 center) const {
        mat4 result;

        result[0][0] = (x * x) - (y * y) - (z * z) + (w * w);
        result[0][1] = 2.0f * ((x * y) + (z * w));
        result[0][2] = 2.0f * ((x * z) - (y * w));
        result[0][3] = center.x - center.x * result[0][0] - center.y * result[0][1] - center.z * result[0][2];

        result[1][0] = 2.0f * ((x * y) - (z * w));
        result[1][1] = -(x * x) + (y * y) - (z * z) + (w * w);
        result[1][2] = 2.0f * ((y * z) + (x * w));
        result[1][3] = center.y - center.x * result[1][0] - center.y * result[1][1] - center.z * result[1][2];

        result[2][0] = 2.0f * ((x * z) + (y * w));
        result[2][1] = 2.0f * ((y * z) - (x * w));
        result[2][2] = -(x * x) - (y * y) + (z * z) + (w * w);
        result[2][3] = center.z - center.x * result[2][0] - center.y * result[2][1] - center.z * result[2][2];

        result[3][0] = 0.0f;
        result[3][1] = 0.0f;
        result[3][2] = 0.0f;
        result[3][3] = 1.0f;

        return result;
    }

    inline static quat from_axis_angle(vec3 axis, float angle, bool normalize) {
        const float half_angle = 0.5f * angle;
        float sin_half_angle = sin(half_angle);
        float cos_half_angle = cos(half_angle);

        quat result = quat(sin_half_angle * axis.x, sin_half_angle * axis.y, sin_half_angle * axis.z, cos_half_angle);
        return normalize ? result.normalized() : result;
    }

    inline static quat slerp(quat from, quat to, float percentage) {
        quat result;

        quat v0 = from.normalized();
        quat v1 = to.normalized();
        float _dot = dot(v0, v1);

        if (_dot < 0.0f) {
            v1.x = -v1.x;
            v1.y = -v1.y;
            v1.z = -v1.z;
            v1.w = -v1.w;
            _dot = -_dot;
        }

        const float DOT_THRESHOLD = 0.9995f;
        if (_dot > DOT_THRESHOLD) {
            // if the inputs are too close together, just lerp and normalize the result
            return quat(
                v0.x + ((v1.x - v0.x) * percentage),
                v0.y + ((v1.y - v0.y) * percentage),
                v0.z + ((v1.z - v0.z) * percentage),
                v0.w + ((v1.w - v0.w) * percentage)
            ).normalized();
        }

        float theta_0 = acos(_dot);
        float theta = theta_0 * percentage;
        float sin_theta = sin(theta);
        float sin_theta_0 = sin(theta_0);

        float s0 = cos(theta) - _dot * sin_theta / sin_theta_0;
        float s1 = sin_theta / sin_theta_0;

        return quat(
            (v0.x * s0) + (v1.x * s1),
            (v0.y * s0) + (v1.y * s1),
            (v0.z * s0) + (v1.z * s1),
            (v0.w * s0) + (v1.w * s1)
        );
    }
};