#include "Mesh.h"

#include "Draw.h"

Mesh::Mesh() {
	mPosAttrib = new Attribute<vec3>();
	mNormAttrib = new Attribute<vec3>();
	mUvAttrib = new Attribute<vec2>();
}

Mesh::Mesh(const Mesh& other) {
}

Mesh& Mesh::operator=(const Mesh& other) {
	if (this == &other) {
		return *this;
	}
	// TODO:
	return *this;
}

Mesh::~Mesh() {
}

void Mesh::updateOpenGLBuffers() {
}

void Mesh::Bind(int position, int normal, int texCoord,
		int weight, int influence) {
	if (position >= 0) {
		mPosAttrib->BindTo(position);
	}
}

void Mesh::UnBind(int position, int normal, int texCoord,
		int weight, int influence) {
}

void Mesh::Draw() {
	if (mIndices.size() > 0) {
		::Draw(*mIndexBuffer, DrawMode::Triangles);
	}
	else {
		::Draw(mPosition.size(), DrawMode::Triangles);
	}
}

