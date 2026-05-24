#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"

// ─── Light types ─────────────────────────────────────────────────────────────

struct DirectionalLight {
    glm::vec3 direction = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.4f));
    glm::vec3 color     = glm::vec3(1.0f, 0.95f, 0.8f);
    float     intensity = 0.6f;
};

struct PointLight {
    glm::vec3 position;
    glm::vec3 color     = glm::vec3(1.0f);
    float     intensity = 1.0f;

    // Attenuation
    float constant  = 1.0f;
    float linear    = 0.09f;
    float quadratic = 0.032f;
};

// ─── Scene object ────────────────────────────────────────────────────────────

struct SceneObject {
    Mesh*     mesh;
    glm::mat4 transform = glm::mat4(1.0f);
};

// ─── Renderer ────────────────────────────────────────────────────────────────

class Renderer {
public:
    Renderer(int viewportWidth, int viewportHeight);

    // Scene management
    void addObject(Mesh* mesh, const glm::mat4& transform = glm::mat4(1.0f));
    void setDirectionalLight(const DirectionalLight& light);
    void addPointLight(const PointLight& light);
    void clearPointLights();

    // Render
    void render(const Camera& camera);
    void resize(int width, int height);

    // Wireframe toggle
    bool wireframe = false;

private:
    int width, height;

    std::unique_ptr<Shader> phongShader;
    std::unique_ptr<Shader> lightBulbShader;

    std::vector<SceneObject>  objects;
    DirectionalLight          dirLight;
    std::vector<PointLight>   pointLights;

    // Light visualiser geometry
    std::unique_ptr<Mesh> lightSphere;

    void uploadLights(Shader& shader) const;
};
