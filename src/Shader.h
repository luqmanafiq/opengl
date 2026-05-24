#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    void use() const;

    void setBool (const std::string& name, bool            value) const;
    void setInt  (const std::string& name, int             value) const;
    void setFloat(const std::string& name, float           value) const;
    void setVec2 (const std::string& name, const glm::vec2& v)   const;
    void setVec3 (const std::string& name, const glm::vec3& v)   const;
    void setVec4 (const std::string& name, const glm::vec4& v)   const;
    void setMat3 (const std::string& name, const glm::mat3& m)   const;
    void setMat4 (const std::string& name, const glm::mat4& m)   const;

private:
    static std::string   loadSource(const char* path);
    static unsigned int  compileShader(const std::string& src, unsigned int type);
    static void          checkErrors(unsigned int id, bool isProgram);
};
