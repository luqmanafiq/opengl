#include "Renderer.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <algorithm>

Renderer::Renderer(int viewportWidth, int viewportHeight)
    : width(viewportWidth), height(viewportHeight)
{
    phongShader     = std::make_unique<Shader>("shaders/phong.vert",     "shaders/phong.frag");
    lightBulbShader = std::make_unique<Shader>("shaders/lightbulb.vert", "shaders/lightbulb.frag");

    lightSphere = std::make_unique<Mesh>(Mesh::makeSphere(12, 12));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void Renderer::addObject(Mesh* mesh, const glm::mat4& transform) {
    objects.push_back({ mesh, transform });
}

void Renderer::setDirectionalLight(const DirectionalLight& light) {
    dirLight = light;
}

void Renderer::addPointLight(const PointLight& light) {
    pointLights.push_back(light);
}

void Renderer::clearPointLights() {
    pointLights.clear();
}

void Renderer::resize(int w, int h) {
    width  = w;
    height = h;
    glViewport(0, 0, w, h);
}

// ─── Main render loop ────────────────────────────────────────────────────────

void Renderer::render(const Camera& camera) {
    glClearColor(0.07f, 0.07f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

    float aspect = (float)width / (float)std::max(height, 1);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = camera.getProjectionMatrix(aspect);

    // ── Pass 1: Phong-lit objects ─────────────────────────────────────────
    phongShader->use();
    phongShader->setMat4("view",       view);
    phongShader->setMat4("projection", proj);
    phongShader->setVec3("viewPos",    camera.position);

    uploadLights(*phongShader);

    for (const auto& obj : objects) {
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(obj.transform)));

        phongShader->setMat4("model",        obj.transform);
        phongShader->setMat3("normalMatrix", normalMatrix);

        // Material
        phongShader->setVec3 ("material.ambient",   obj.mesh->material.ambient);
        phongShader->setVec3 ("material.diffuse",   obj.mesh->material.diffuse);
        phongShader->setVec3 ("material.specular",  obj.mesh->material.specular);
        phongShader->setFloat("material.shininess", obj.mesh->material.shininess);

        obj.mesh->draw();
    }

    // ── Pass 2: Light-bulb visualisers ────────────────────────────────────
    lightBulbShader->use();
    lightBulbShader->setMat4("view",       view);
    lightBulbShader->setMat4("projection", proj);

    for (const auto& pl : pointLights) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), pl.position);
        model           = glm::scale(model, glm::vec3(0.12f));
        lightBulbShader->setMat4("model",      model);
        lightBulbShader->setVec3("lightColor", pl.color * pl.intensity);
        lightSphere->draw();
    }
}

// ─── Upload lights to a shader ───────────────────────────────────────────────

void Renderer::uploadLights(Shader& shader) const {
    // Directional light
    shader.setVec3 ("dirLight.direction", dirLight.direction);
    shader.setVec3 ("dirLight.color",     dirLight.color * dirLight.intensity);

    // Point lights
    int count = (int)std::min(pointLights.size(), (size_t)8); // max 8 point lights
    shader.setInt("numPointLights", count);
    for (int i = 0; i < count; ++i) {
        std::string base = "pointLights[" + std::to_string(i) + "].";
        shader.setVec3 (base + "position",  pointLights[i].position);
        shader.setVec3 (base + "color",     pointLights[i].color * pointLights[i].intensity);
        shader.setFloat(base + "constant",  pointLights[i].constant);
        shader.setFloat(base + "linear",    pointLights[i].linear);
        shader.setFloat(base + "quadratic", pointLights[i].quadratic);
    }
}
