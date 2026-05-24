#pragma once
#include <vector>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Material {
    glm::vec3 ambient  = glm::vec3(0.1f);
    glm::vec3 diffuse  = glm::vec3(0.8f);
    glm::vec3 specular = glm::vec3(1.0f);
    float     shininess = 32.0f;
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices,
         const std::vector<unsigned int>& indices,
         const Material& material = {});
    ~Mesh();

    void draw() const;

    Material material;

    // Factory helpers
    static Mesh makeCube();
    static Mesh makeSphere(unsigned int rings = 32, unsigned int sectors = 32);
    static Mesh makePlane(float size = 10.0f, unsigned int subdivisions = 1);

private:
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;

    void setup(const std::vector<Vertex>& vertices,
               const std::vector<unsigned int>& indices);
};
