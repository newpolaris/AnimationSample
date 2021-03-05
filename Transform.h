#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "Vec3.h"
#include "Quat.h"

struct Transform {
    vec3 position;
    quat rotation;
    vec3 scale;
public:
    Transform();
    Transform(const vec3& p, const quat& r, const vec3& s);
};

Transform combine(const Transform& a, const Transform& b);

vec3 transformPoint(const Transform& a, const vec3& b);
vec3 transformVector(const Transform& a, const vec3& b);

#endif // __TRANSFORM_H__

