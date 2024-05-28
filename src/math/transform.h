#pragma once

#include "vector3.h"
#include "matrix.h"
#include "quaternion.h"

struct Transform {
    vec3 origin;
    quat rotation;
    vec3 scale;

    inline mat4 to_mat4() const {
        return mat4::translate(origin) * rotation.to_mat4() * mat4::scale(scale);
    }
};