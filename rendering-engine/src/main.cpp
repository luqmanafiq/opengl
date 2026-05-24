#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <stdexcept>
#include <memory>
#include <cmath>

#include "Camera.h"
#include "Mesh.h"
#include "Renderer.h"

// ─── Window settings ─────────────────────────────────────────────────────────

static constexpr int    INITIAL_WIDTH  = 1280;
static constexpr int    INITIAL_HEIGHT = 720;
static constexpr char   WINDOW_TITLE[] = "LuqRenderer";

// ─── Globals (only for GLFW callbacks) ───────────────────────────────────────

static Camera   g_camera(glm::vec3(0.0f, 2.0f, 8.0f));
static Renderer* g_renderer = nullptr;

static float g_lastX      = INITIAL_WIDTH  / 2.0f;
static float g_lastY      = INITIAL_HEIGHT / 2.0f;
static bool  g_firstMouse = true;
static bool  g_cursorLocked = true;

// ─── GLFW callbacks ───────────────────────────────────────────────────────────

static void framebufferSizeCallback(GLFWwindow*, int w, int h) {
    if (g_renderer) g_renderer->resize(w, h);
}

static void scrollCallback(GLFWwindow*, double, double yOffset) {
    g_camera.processMouseScroll((float)yOffset);
}

static void mouseCallback(GLFWwindow*, double xpos, double ypos) {
    if (!g_cursorLocked) return;
    if (g_firstMouse) {
        g_lastX      = (float)xpos;
        g_lastY      = (float)ypos;
        g_firstMouse = false;
    }
    float xOff = (float)xpos - g_lastX;
    float yOff = g_lastY - (float)ypos;   // y is flipped
    g_lastX = (float)xpos;
    g_lastY = (float)ypos;
    g_camera.processMouseMove(xOff, yOff);
}

static void keyCallback(GLFWwindow* window, int key, int, int action, int) {
    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_ESCAPE) {
        g_cursorLocked = !g_cursorLocked;
        glfwSetInputMode(window, GLFW_CURSOR,
                         g_cursorLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
    if (key == GLFW_KEY_F && g_renderer)
        g_renderer->wireframe = !g_renderer->wireframe;
}

// ─── Input polling (called every frame) ──────────────────────────────────────

static void processInput(GLFWwindow* window, float dt) {
    if (!g_cursorLocked) return;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) g_camera.processKeyboard(CameraMovement::FORWARD,  dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) g_camera.processKeyboard(CameraMovement::BACKWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) g_camera.processKeyboard(CameraMovement::LEFT,     dt);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) g_camera.processKeyboard(CameraMovement::RIGHT,    dt);
    if (glfwGetKey(window, GLFW_KEY_SPACE)      == GLFW_PRESS) g_camera.processKeyboard(CameraMovement::UP,   dt);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) g_camera.processKeyboard(CameraMovement::DOWN, dt);
}

// ─── Scene builder ────────────────────────────────────────────────────────────

static void buildScene(Renderer& renderer,
                        std::vector<std::unique_ptr<Mesh>>& meshes)
{
    // Materials
    Material chrome{
        .ambient   = glm::vec3(0.05f),
        .diffuse   = glm::vec3(0.55f, 0.55f, 0.60f),
        .specular  = glm::vec3(0.90f),
        .shininess = 128.0f
    };
    Material red{
        .ambient   = glm::vec3(0.05f, 0.01f, 0.01f),
        .diffuse   = glm::vec3(0.80f, 0.10f, 0.10f),
        .specular  = glm::vec3(0.50f),
        .shininess = 64.0f
    };
    Material gold{
        .ambient   = glm::vec3(0.10f, 0.07f, 0.01f),
        .diffuse   = glm::vec3(0.85f, 0.65f, 0.10f),
        .specular  = glm::vec3(0.90f, 0.75f, 0.30f),
        .shininess = 96.0f
    };
    Material floor{
        .ambient   = glm::vec3(0.05f),
        .diffuse   = glm::vec3(0.40f, 0.40f, 0.42f),
        .specular  = glm::vec3(0.20f),
        .shininess = 16.0f
    };

    // ── Sphere (centre) ──────────────────────────────────────────────────
    auto sphere = std::make_unique<Mesh>(Mesh::makeSphere(48, 48));
    sphere->material = chrome;
    glm::mat4 sphereT = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    sphereT           = glm::scale(sphereT, glm::vec3(1.5f));
    renderer.addObject(sphere.get(), sphereT);
    meshes.push_back(std::move(sphere));

    // ── Red cube (left) ───────────────────────────────────────────────────
    auto cube = std::make_unique<Mesh>(Mesh::makeCube());
    cube->material = red;
    glm::mat4 cubeT = glm::translate(glm::mat4(1.0f), glm::vec3(-3.2f, 0.5f, 0.0f));
    cubeT           = glm::rotate(cubeT, glm::radians(22.0f), glm::vec3(0,1,0));
    renderer.addObject(cube.get(), cubeT);
    meshes.push_back(std::move(cube));

    // ── Gold cube (right) ─────────────────────────────────────────────────
    auto cube2 = std::make_unique<Mesh>(Mesh::makeCube());
    cube2->material = gold;
    glm::mat4 cubeT2 = glm::translate(glm::mat4(1.0f), glm::vec3(3.2f, 0.5f, 0.0f));
    cubeT2           = glm::rotate(cubeT2, glm::radians(-15.0f), glm::vec3(0,1,0));
    renderer.addObject(cube2.get(), cubeT2);
    meshes.push_back(std::move(cube2));

    // ── Small spheres (orbit) ──────────────────────────────────────────────
    for (int i = 0; i < 5; ++i) {
        float angle = glm::radians(i * 72.0f);
        glm::vec3 pos = { std::cos(angle) * 4.5f, 0.4f, std::sin(angle) * 4.5f };
        auto small = std::make_unique<Mesh>(Mesh::makeSphere(16, 16));
        small->material = { glm::vec3(0.04f), glm::vec3(0.2f, 0.4f+i*0.1f, 0.7f), glm::vec3(0.6f), 48.0f };
        renderer.addObject(small.get(),
            glm::scale(glm::translate(glm::mat4(1.0f), pos), glm::vec3(0.5f)));
        meshes.push_back(std::move(small));
    }

    // ── Ground plane ──────────────────────────────────────────────────────
    auto plane = std::make_unique<Mesh>(Mesh::makePlane(20.0f, 4));
    plane->material = floor;
    renderer.addObject(plane.get(), glm::mat4(1.0f));
    meshes.push_back(std::move(plane));

    // ── Directional (sun) ─────────────────────────────────────────────────
    renderer.setDirectionalLight({
        .direction = glm::normalize(glm::vec3(-0.4f, -1.0f, -0.5f)),
        .color     = glm::vec3(1.0f, 0.97f, 0.88f),
        .intensity = 0.5f
    });

    // ── Point lights ──────────────────────────────────────────────────────
    renderer.addPointLight({ .position = {-3.0f, 3.0f,  2.0f}, .color = {1.0f, 0.3f, 0.2f}, .intensity = 4.0f });
    renderer.addPointLight({ .position = { 3.0f, 3.0f,  2.0f}, .color = {0.2f, 0.5f, 1.0f}, .intensity = 4.0f });
    renderer.addPointLight({ .position = { 0.0f, 4.0f, -3.0f}, .color = {0.3f, 1.0f, 0.5f}, .intensity = 3.0f });
}

// ─── Entry point ─────────────────────────────────────────────────────────────

int main() {
    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(INITIAL_WIDTH, INITIAL_HEIGHT,
                                          WINDOW_TITLE, nullptr, nullptr);
    if (!window) {
        std::cerr << "GLFW window creation failed\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window,       mouseCallback);
    glfwSetScrollCallback(window,          scrollCallback);
    glfwSetKeyCallback(window,             keyCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD init failed\n";
        return 1;
    }

    glEnable(GL_MULTISAMPLE);
    std::cout << "OpenGL " << glGetString(GL_VERSION) << '\n';
    std::cout << "Controls: WASD = move | Mouse = look | Scroll = zoom\n";
    std::cout << "          ESC = toggle cursor lock | F = toggle wireframe\n";

    try {
        Renderer renderer(INITIAL_WIDTH, INITIAL_HEIGHT);
        g_renderer = &renderer;

        std::vector<std::unique_ptr<Mesh>> meshes;
        buildScene(renderer, meshes);

        float lastFrame = 0.0f;

        while (!glfwWindowShouldClose(window)) {
            float now = (float)glfwGetTime();
            float dt  = now - lastFrame;
            lastFrame = now;

            processInput(window, dt);

            // Animate point light positions
            renderer.clearPointLights();
            float s = std::sin(now * 0.8f);
            float c = std::cos(now * 0.8f);
            renderer.addPointLight({ .position = {-3.0f + s, 3.0f,  2.0f + c}, .color = {1.0f, 0.3f, 0.2f}, .intensity = 4.0f });
            renderer.addPointLight({ .position = { 3.0f + c, 3.0f,  2.0f + s}, .color = {0.2f, 0.5f, 1.0f}, .intensity = 4.0f });
            renderer.addPointLight({ .position = { s * 2.0f, 4.0f, -3.0f    }, .color = {0.3f, 1.0f, 0.5f}, .intensity = 3.0f });

            renderer.render(g_camera);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << '\n';
        glfwTerminate();
        return 1;
    }

    glfwTerminate();
    return 0;
}
