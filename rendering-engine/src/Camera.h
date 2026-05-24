#pragma once
#include <glm/glm.hpp>

enum class CameraMovement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

class Camera {
public:
    // Transforms
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    // Euler angles
    float yaw;
    float pitch;

    // Settings
    float movementSpeed  = 5.0f;
    float mouseSensitivity = 0.1f;
    float fov            = 45.0f;

    explicit Camera(glm::vec3 position   = glm::vec3(0.0f, 0.0f, 3.0f),
                    glm::vec3 up         = glm::vec3(0.0f, 1.0f, 0.0f),
                    float     yaw        = -90.0f,
                    float     pitch      = 0.0f);

    glm::mat4 getViewMatrix()       const;
    glm::mat4 getProjectionMatrix(float aspectRatio, float near = 0.1f, float far = 100.0f) const;

    void processKeyboard  (CameraMovement dir, float dt);
    void processMouseMove (float xOffset, float yOffset, bool constrainPitch = true);
    void processMouseScroll(float yOffset);

private:
    void updateVectors();
};
