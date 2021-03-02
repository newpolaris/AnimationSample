#include "Draw.h"

#include <iostream>
#include "glad.h"

static GLenum DrawModeToGLEnum(DrawMode input) {
    switch (input) {
    case DrawMode::Points: return GL_POINTS;
    case DrawMode::LineStrip: return GL_LINE_STRIP;
    case DrawMode::LineLoop: return GL_LINE_LOOP;
    case DrawMode::Lines: return GL_LINES;
    case DrawMode::Triangles: return GL_TRIANGLES;
    case DrawMode::TriangleStrip: return GL_TRIANGLE_STRIP;
    case DrawMode::TriangleFan: return GL_TRIANGLE_FAN;
    }
    std::cout << "DrawModeToGLEnum unreadchable code hit\n";
    return 0;
}

void Draw(unsigned int vertexCount, DrawMode mode) {
    glDrawArrays(DrawModeToGLEnum(mode), 0, vertexCount);
}

void DrawInstance(unsigned int vertexCount,
        DrawMode mode, unsigned int numInstances) {
    glDrawArraysInstanced(DrawModeToGLEnum(mode),
            0, vertexCount, numInstances);
}

#if 0
void Draw(IndexBuffer& inIndexBuffer, DrawMode mode) {
}

void DrawInstanced(IndexBuffer& inIndexBuffer,
        DrawMode mode, unsigned int instanceCount) {
    unsigned int handle = inIndexBuffer.GetHandle();
    unsigned int numIndices = inIndexBuffer.Count();
}
#endif
