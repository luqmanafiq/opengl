#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

Camera::Camera(glm::vec3 pos, glm::vec3 worldUpVec, float yawAngle, float pitchAngle)
    : position(pos), worldUp(worldUpVec), yaw(yawAngle), pitch(pitchAngle)
{
    updateVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio, float near, float far) const {
    return glm::perspective(glm::radians(fov), aspectRatio, near, far);
}

void Camera::processKeyboard(CameraMovement dir, float dt) {
    float velocity = movementSpeed * dt;
    switch (dir) {
        case CameraMovement::FORWARD:  position += front * velocity;  break;
        case CameraMovement::BACKWARD: position -= front * velocity;  break;
        case CameraMovement::LEFT:     position -= right * velocity;  break;
        case CameraMovement::RIGHT:    position += right * velocity;  break;
        case CameraMovement::UP:       position += worldUp * velocity; break;
        case CameraMovement::DOWN:     position -= worldUp * velocity; break;
    }
}

void Camera::processMouseMove(float xOffset, float yOffset, bool constrainPitch) {
    yaw   += xOffset * mouseSensitivity;
    pitch += yOffset * mouseSensitivity;
    if (constrainPitch)
        pitch = std::clamp(pitch, -89.0f, 89.0f);
    updateVectors();
}

void Camera::processMouseScroll(float yOffset) {
    fov = std::clamp(fov - yOffset, 1.0f, 90.0f);
}

void Camera::updateVectors() {
    glm::vec3 f;
    f.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    f.y = std::sin(glm::radians(pitch));
    f.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    front = glm::normalize(f);
    right = glm::normalize(glm::cross(front, worldUp));
    up    = glm::normalize(glm::cross(right, front));
}
