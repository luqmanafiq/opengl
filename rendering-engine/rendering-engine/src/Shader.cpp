#include "Shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

// ─── Construction ────────────────────────────────────────────────────────────

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    std::string   vertSrc = loadSource(vertexPath);
    std::string   fragSrc = loadSource(fragmentPath);

    unsigned int vert = compileShader(vertSrc, GL_VERTEX_SHADER);
    unsigned int frag = compileShader(fragSrc, GL_FRAGMENT_SHADER);

    ID = glCreateProgram();
    glAttachShader(ID, vert);
    glAttachShader(ID, frag);
    glLinkProgram(ID);
    checkErrors(ID, true);

    glDeleteShader(vert);
    glDeleteShader(frag);
}

Shader::~Shader() {
    glDeleteProgram(ID);
}

// ─── Public API ──────────────────────────────────────────────────────────────

void Shader::use() const { glUseProgram(ID); }

void Shader::setBool (const std::string& n, bool            v) const { glUniform1i (glGetUniformLocation(ID, n.c_str()), (int)v); }
void Shader::setInt  (const std::string& n, int             v) const { glUniform1i (glGetUniformLocation(ID, n.c_str()), v); }
void Shader::setFloat(const std::string& n, float           v) const { glUniform1f (glGetUniformLocation(ID, n.c_str()), v); }
void Shader::setVec2 (const std::string& n, const glm::vec2& v) const { glUniform2fv(glGetUniformLocation(ID, n.c_str()), 1, glm::value_ptr(v)); }
void Shader::setVec3 (const std::string& n, const glm::vec3& v) const { glUniform3fv(glGetUniformLocation(ID, n.c_str()), 1, glm::value_ptr(v)); }
void Shader::setVec4 (const std::string& n, const glm::vec4& v) const { glUniform4fv(glGetUniformLocation(ID, n.c_str()), 1, glm::value_ptr(v)); }
void Shader::setMat3 (const std::string& n, const glm::mat3& m) const { glUniformMatrix3fv(glGetUniformLocation(ID, n.c_str()), 1, GL_FALSE, glm::value_ptr(m)); }
void Shader::setMat4 (const std::string& n, const glm::mat4& m) const { glUniformMatrix4fv(glGetUniformLocation(ID, n.c_str()), 1, GL_FALSE, glm::value_ptr(m)); }

// ─── Private helpers ─────────────────────────────────────────────────────────

std::string Shader::loadSource(const char* path) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error(std::string("Shader file not found: ") + path);
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

unsigned int Shader::compileShader(const std::string& src, unsigned int type) {
    unsigned int id = glCreateShader(type);
    const char* c   = src.c_str();
    glShaderSource(id, 1, &c, nullptr);
    glCompileShader(id);
    checkErrors(id, false);
    return id;
}

void Shader::checkErrors(unsigned int id, bool isProgram) {
    int  success;
    char log[1024];
    if (isProgram) {
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(id, 1024, nullptr, log);
            throw std::runtime_error(std::string("Shader link error:\n") + log);
        }
    } else {
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(id, 1024, nullptr, log);
            throw std::runtime_error(std::string("Shader compile error:\n") + log);
        }
    }
}
