#ifndef __MESH_H__
#define __MESH_H__

#include <vector>

#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"

class Mesh {
protected:
	std::vector<vec3> mPosition;
	std::vector<vec3> mNormal;
	std::vector<vec3> mTexCoord;
	std::vector<vec4> mWeights;
	std::vector<ivec4> mInfluences
	std::vector<unsigned int> mIndices;
protectd:
	Attribute<vec3>* mPosAttrib;
	Attribute<vec3>* mNormAttrib;
	Attribute<vec2>* mUvAttrib;
	Attribute<vec4>* mWeightAttrib;
	Attribute<ivec4>* mInfluenceAttrib;
	IndexBuffer* mIndexBuffer;
protected:
	// additional copy of the pose and normal data, 
	// as well as a matrix palette to use for CPU
	// skinning;
	std::vector<vec3> mSkinnedPosition;
	std::vector<vec3> mSkinnedNormal;
	std::vector<mat4> mPosePalette;
public:
};

#endif // __MESH_H__
