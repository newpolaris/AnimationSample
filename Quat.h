#ifndef __QUAT_H__
#define __QUAT_H__

#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"

struct quat {
    union {
        struct { float x, y, z, w; };
        vec4 xyzw;
        vec3 xyz;
        vec2 xy;
        struct {
            vec3 vector;
            float scalar;
        };
        float v[4];
    };
    quat();
    quat(const vec3& v, float w);
    quat(float x, float y, float z, float w);
};

quat operator*(const quat& q, const quat& r);
vec3 operator*(const quat& q, const vec3& v);

quat angleAxis(float angle, const vec3& axis);

#endif // __QUAT_H__
