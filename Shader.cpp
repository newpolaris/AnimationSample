#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include "glad.h"

Shader::Shader() {
    mHandle = glCreateProgram();
}

Shader::Shader(const std::string& vertex,
               const std::string& fragment) {
    mHandle = glCreateProgram();
    Load(vertex, fragment);
}

Shader::~Shader() {
    glDeleteProgram(mHandle);
}

std::string Shader::ReadFile(const std::string& path) {
    std::ifstream file;
    file.open(path);
    std::stringstream contents;
    contents << file.rdbuf();
    file.close();
    return contents.str();
}

unsigned int Shader::CompileVertexShader(
        const std::string& vertex) {
    unsigned int  v = glCreateShader(GL_VERTEX_SHADER);
    const char* v_source = vertex.c_str();
    glShaderSource(v, 1, &v_source, NULL);
    glCompileShader(v);
    int success = 0;
    glGetShaderiv(v, GL_COMPILE_STATUS, &success);
    if (!success) {
        static const size_t kLen = 512;
        char infoLog[kLen];
        glGetShaderInfoLog(v, kLen, NULL, infoLog);
        std::cout << "Vertex compilelation failed.\n";
        std::cout << "\t" << infoLog << "\n";
        glDeleteShader(v);
        return 0;
    }
    return v;
}

unsigned int Shader::CompileFragmentShader(
        const std::string& fragment) {
    unsigned int f = glCreateShader(GL_FRAGMENT_SHADER);
    const char* f_source = fragment.c_str();
    glShaderSource(f, 1, &f_source, NULL);
    glCompileShader(f);
    int success = 0;
    glGetShaderiv(f, GL_COMPILE_STATUS, &success);
    if (!success) {
        static const size_t kLen = 512;
        char infoLog[kLen];
        glGetShaderInfoLog(f, kLen, NULL, infoLog);
        std::cout << "Vertex compilelation failed.\n";
        std::cout << "\t" << infoLog << "\n";
        glDeleteShader(f);
        return 0;
    }
    return f;
}

bool Shader::LinkShaders(unsigned int vertex,
                         unsigned int fragment) {
    glAttachShader(mHandle, vertex);
    glAttachShader(mHandle, fragment);
    glLinkProgram(mHandle);
    int success = 0;
    glGetProgramiv(mHandle, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(mHandle, 512, NULL, infoLog);
        std::cout << "ERROR: Shader linking failed.\n";
        std::cout << "\t" << infoLog << "\n";
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return false;
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return true;
}

void Shader::PopulateAttributes() {
    static const size_t kLen = 128;
    int count = -1;
    int length;
    char name[kLen];
    int size;
    GLenum type;
    glUseProgram(mHandle);
    glGetProgramiv(mHandle, GL_ACTIVE_ATTRIBUTES, &count);
    for (int i = 0; i < count; ++i) {
        memset(name, 0, sizeof(char) * kLen);
        glGetActiveAttrib(mHandle, (GLuint)i, 128,
                &length, &size, &type, name);
        int attrib = glGetAttribLocation(mHandle, name);
        if (attrib >= 0) {
            mAttributes[name] = attrib;
        }
    }
    glUseProgram(0);
}

void Shader::PopulateUniforms() {
    static const size_t kName = 128;
    static const size_t kTempName = 256;
    int count = -1;
    int length;
    char name[kName];
    int size;
    GLenum type;
    char testName[kTempName];
    glUseProgram(mHandle);
    glGetProgramiv(mHandle, GL_ACTIVE_UNIFORMS, &count);
    for (size_t i = 0; i < count; i++) {
        memset(name, 0, sizeof(name));
        glGetActiveUniform(mHandle, (GLuint)i, kName,
                &length, &size, &type, name);
        int uniform = glGetUniformLocation(mHandle, name);
        if (uniform >= 0) { // Is Uniform valid?
            std::string uniformName = name;
            // if name contains [, uniform is array
            std::size_t found = uniformName.find('[');
            if (found != std::string::npos) {
                uniformName.erase(uniformName.begin() + found,
                        uniformName.end());
                unsigned int uniformIndex = 0;
                while (true) {
                    memset(testName, 0, sizeof(testName));
                    sprintf(testName, "%s[%d]", uniformName.c_str(),
                            uniformIndex++);
                    int uniformLocation = glGetUniformLocation(
                            mHandle, testName);
                    if (uniformLocation < 0) {
                        break;
                    }
                    mUniforms[testName] = uniformLocation;
                }
            }
            mUniforms[uniformName] = uniform;
        }
    }
    glUseProgram(0);
}

void Shader::Load(const std::string& vertex,
                  const std::string& fragment) {
    std::ifstream vf(vertex.c_str());
    bool vertFile = vf.good();
    vf.close();
    std::ifstream ff(vertex.c_str());
    bool fragFile = ff.good();
    ff.close();
    std::string v_source = vertex;
    if (vertFile) {
        v_source = ReadFile(vertex);
    }
    std::string f_source = fragment;
    if (fragFile) {
        f_source = ReadFile(fragment);
    }
    unsigned int vert = CompileVertexShader(v_source);
    unsigned int frag = CompileFragmentShader(f_source);
    if (LinkShaders(vert, frag)) {
        PopulateAttributes();
        PopulateUniforms();
    }
}

void Shader::Bind() {
    glUseProgram(mHandle);
}

void Shader::UnBind() {
    glUseProgram(0);
}

unsigned int Shader::GetHandle() {
    return mHandle;
}

unsigned int Shader::GetAttribute(const std::string& name) {
    return 0;
}
