#include "Transform.h"

Transform::Transform(const vec3& p, const quat& r, const vec3& s) :
    position(p), rotation(r), scale(s) 
{}

Transform::Transform() :
    position(vec3(0, 0, 0)), 
    rotation(quat(0, 0, 0, 1)), 
    scale(vec3(1, 1, 1)) 
{}

/*
 *  quat q(vec3(0, 1, 0), 3.14f / 4);
 *  Transform a(vec3(0, 0, 3), q, vec3(3.f));
 *  Transform b(vec3(3, 0, 0), quat(), vec3(1.f));
 *
 *  auto p = vec3(1, 1, 1);
 *  auto r0 = transformPoint(a, transformPoint(b, p));
 *  auto r1 = transformPoint(combine(a, b), p);
 */

Transform combine(const Transform& a, const Transform& b) {
    Transform out;
    out.scale = b.scale * a.scale;
    out.rotation = b.rotation * a.rotation;
    out.position = a.rotation * (a.scale * b.position);
    out.position = a.position + out.position;
    return out;
}

vec3 transformPoint(const Transform& a, const vec3& b) {
    vec3 out;
    out = a.rotation * (a.scale * b);
    out = a.position + out;
    return out;
}

vec3 transformVector(const Transform& a, const vec3& b)
{
    vec3 out;
    out = a.rotation * (a.scale * b);
    return out;
}
