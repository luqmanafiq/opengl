#include "Mesh.h"

#include <glad/glad.h>
#include <glm/gtc/constants.hpp>
#include <cmath>

// ─── Lifecycle ───────────────────────────────────────────────────────────────

Mesh::Mesh(const std::vector<Vertex>& vertices,
           const std::vector<unsigned int>& indices,
           const Material& mat)
    : material(mat), indexCount((unsigned int)indices.size())
{
    setup(vertices, indices);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

// ─── Draw ────────────────────────────────────────────────────────────────────

void Mesh::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

// ─── Private setup ───────────────────────────────────────────────────────────

void Mesh::setup(const std::vector<Vertex>& vertices,
                 const std::vector<unsigned int>& indices) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(Vertex),
                 vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, position));
    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, normal));
    // texCoords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, texCoords));

    glBindVertexArray(0);
}

// ─── Factory: Cube ───────────────────────────────────────────────────────────

Mesh Mesh::makeCube() {
    std::vector<Vertex> vertices = {
        // Front
        {{-0.5f,-0.5f, 0.5f},{0,0,1},{0,0}}, {{ 0.5f,-0.5f, 0.5f},{0,0,1},{1,0}},
        {{ 0.5f, 0.5f, 0.5f},{0,0,1},{1,1}}, {{-0.5f, 0.5f, 0.5f},{0,0,1},{0,1}},
        // Back
        {{ 0.5f,-0.5f,-0.5f},{0,0,-1},{0,0}}, {{-0.5f,-0.5f,-0.5f},{0,0,-1},{1,0}},
        {{-0.5f, 0.5f,-0.5f},{0,0,-1},{1,1}}, {{ 0.5f, 0.5f,-0.5f},{0,0,-1},{0,1}},
        // Left
        {{-0.5f,-0.5f,-0.5f},{-1,0,0},{0,0}}, {{-0.5f,-0.5f, 0.5f},{-1,0,0},{1,0}},
        {{-0.5f, 0.5f, 0.5f},{-1,0,0},{1,1}}, {{-0.5f, 0.5f,-0.5f},{-1,0,0},{0,1}},
        // Right
        {{ 0.5f,-0.5f, 0.5f},{1,0,0},{0,0}}, {{ 0.5f,-0.5f,-0.5f},{1,0,0},{1,0}},
        {{ 0.5f, 0.5f,-0.5f},{1,0,0},{1,1}}, {{ 0.5f, 0.5f, 0.5f},{1,0,0},{0,1}},
        // Top
        {{-0.5f, 0.5f, 0.5f},{0,1,0},{0,0}}, {{ 0.5f, 0.5f, 0.5f},{0,1,0},{1,0}},
        {{ 0.5f, 0.5f,-0.5f},{0,1,0},{1,1}}, {{-0.5f, 0.5f,-0.5f},{0,1,0},{0,1}},
        // Bottom
        {{-0.5f,-0.5f,-0.5f},{0,-1,0},{0,0}}, {{ 0.5f,-0.5f,-0.5f},{0,-1,0},{1,0}},
        {{ 0.5f,-0.5f, 0.5f},{0,-1,0},{1,1}}, {{-0.5f,-0.5f, 0.5f},{0,-1,0},{0,1}},
    };

    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < 6; ++i) {
        unsigned int base = i * 4;
        indices.insert(indices.end(), {base,base+1,base+2, base,base+2,base+3});
    }
    return Mesh(vertices, indices);
}

// ─── Factory: Sphere ─────────────────────────────────────────────────────────

Mesh Mesh::makeSphere(unsigned int rings, unsigned int sectors) {
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;

    const float PI = glm::pi<float>();

    for (unsigned int r = 0; r <= rings; ++r) {
        float phi = PI * r / rings;          // 0 → π
        for (unsigned int s = 0; s <= sectors; ++s) {
            float theta = 2.0f * PI * s / sectors; // 0 → 2π

            glm::vec3 pos{
                std::sin(phi) * std::cos(theta),
                std::cos(phi),
                std::sin(phi) * std::sin(theta)
            };
            vertices.push_back({ pos * 0.5f, pos, { (float)s / sectors, (float)r / rings } });
        }
    }

    for (unsigned int r = 0; r < rings; ++r) {
        for (unsigned int s = 0; s < sectors; ++s) {
            unsigned int cur  = r * (sectors + 1) + s;
            unsigned int next = cur + sectors + 1;
            indices.insert(indices.end(), { cur, next, cur+1, next, next+1, cur+1 });
        }
    }
    return Mesh(vertices, indices);
}

// ─── Factory: Plane ──────────────────────────────────────────────────────────

Mesh Mesh::makePlane(float size, unsigned int subdivisions) {
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;

    float step = size / subdivisions;
    float half = size * 0.5f;

    for (unsigned int z = 0; z <= subdivisions; ++z) {
        for (unsigned int x = 0; x <= subdivisions; ++x) {
            float px = -half + x * step;
            float pz = -half + z * step;
            float u  = (float)x / subdivisions;
            float v  = (float)z / subdivisions;
            vertices.push_back({ {px, 0.0f, pz}, {0,1,0}, {u,v} });
        }
    }

    unsigned int w = subdivisions + 1;
    for (unsigned int z = 0; z < subdivisions; ++z) {
        for (unsigned int x = 0; x < subdivisions; ++x) {
            unsigned int i = z * w + x;
            indices.insert(indices.end(), { i, i+w, i+1, i+w, i+w+1, i+1 });
        }
    }
    return Mesh(vertices, indices);
}
