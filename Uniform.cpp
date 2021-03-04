#include "Uniform.h"

#include "glad.h"

#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Mat4.h"

template Uniform<int>;
template Uniform<float>;
template Uniform<ivec2>;
template Uniform<ivec4>;
template Uniform<vec2>;
template Uniform<vec3>;
template Uniform<vec4>;
// template Uniform<quat>;
template Uniform<mat4>;

template <typename T>
void Uniform<T>::Set(unsigned int slot, const T& value) {
	Set(slot, (const T*)&value, 1);
}

template <typename T>
void Uniform<T>::Set(unsigned int s, const std::vector<T>& v) {
	Set(s, v.data(), (unsigned int)v.size());
}

#define UNIFORM_IMPL(gl_func, tType, dType) 			\
template<> void Uniform<tType>::Set(unsigned int slot,	\
			 const tType* data, unsigned int length) {	\
	gl_func(slot, (GLsizei)length, (const dType*)data);	\
}

UNIFORM_IMPL(glUniform1iv, int, int)
UNIFORM_IMPL(glUniform2iv, ivec2, int)
UNIFORM_IMPL(glUniform4iv, ivec4, int)
UNIFORM_IMPL(glUniform1fv, float, float)
UNIFORM_IMPL(glUniform2fv, vec2, float)
UNIFORM_IMPL(glUniform3fv, vec3, float)
UNIFORM_IMPL(glUniform4fv, vec4, float)
// UNIFORM_IMPL(glUniform4fv, quat, float)

template<> void Uniform<mat4>::Set(unsigned int slot,
	const mat4* inputArray, unsigned int arrayLength) {
	glUniformMatrix4fv(slot, (GLsizei)arrayLength,
		false, (const float*)inputArray);
}
