#include "Quat.h"
#include "Vec3.h"

quat::quat() 
    : x(0), y(0), z(0), w(1) 
{
}

quat::quat(float x, float y, float z, float w) 
    : x(x), y(y), z(z), w(w) 
{
}

quat::quat(const vec3& v, float w)
    : vector{ v }, scalar(w)
{
}

/*
 * quat s(vec3(0, 0, 1), 3.14f / 4);
 * quat r(vec3(0, 1, 0), 3.14f / 4);
 * vec3 p(0, -1, 0);
 * auto r0 = s * (r * p);
 * auto r1 = (r * s) * p;
 */
quat operator*(const quat& q, const quat& r)
{
    // 
    // q * (r * vec3) = (r * q) * vec3
    // 
    // return quat(
    //          q.xyz*r.w + r.xyz*q.w + cross(r.xyz, q.xyz),
    //          r.w*q.w - dot(r.xyz, q.xyz));
    return quat(
         r.x * q.w + r.y * q.z - r.z * q.y + r.w * q.x,
        -r.x * q.z + r.y * q.w + r.z * q.x + r.w * q.y,
         r.x * q.y - r.y * q.x + r.z * q.w + r.w * q.z,
        -r.x * q.x - r.y * q.y - r.z * q.z + r.w * q.w
    );
}

vec3 operator*(const quat& q, const vec3& v) {
    return q.vector * 2.0f * dot(q.vector, v)
        + v * (q.scalar * q.scalar - dot(q.vector, q.vector))
        + cross(q.vector, v) * 2.0f * q.scalar;
}

quat angleAxis(float angle, const vec3& axis) {
    vec3 norm = normalized(axis);
    float s = sinf(angle * 0.5f);
    return quat(norm * s, cosf(angle * 0.5f));
}

