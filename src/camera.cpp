#include "camera.h"

#include "application.h"
#include "events/dispatcher.h"

#include <glm/gtc/matrix_transform.hpp>


Camera::Camera(glm::vec3 position) : front(glm::vec3(0.0f, 0.0f, -1.0f)), worldUp(glm::vec3(0.0f, 1.0f, 0.0f)), yaw(YAW), pitch(PITCH), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
    this->position = position;
    updateCameraVectors();

    Dispatcher::instance().subscribe(MouseScrollEvent::descriptor,
         std::bind(&Camera::onMouseScroll, this, std::placeholders::_1));
    Dispatcher::instance().subscribe(MouseMoveEvent::descriptor,
        std::bind(&Camera::onMouseMove, this, std::placeholders::_1));
    Dispatcher::instance().subscribe(KeyPressEvent::descriptor,
             std::bind(&Camera::onKeyPress, this, std::placeholders::_1));
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(position, position + front, up);
}

void Camera::onKeyPress(const Event& e)
{
    const auto& event = static_cast<const KeyPressEvent&>(e);
    int keyCode = event.keyCode();
    int modifier = event.modifier();

    if (modifier != 342)
        return;

    float velocity = movementSpeed * Application::deltaTime;
    if (keyCode == 87)
        position += front * velocity;
    else if (keyCode == 83)
        position -= front * velocity;
    else if (keyCode == 65)
        position -= right * velocity;
    else if (keyCode == 68)
        position += right * velocity;
    else if (keyCode == 81)
        position -= up * velocity;
    else if (keyCode == 69)
        position += up * velocity;
}

void Camera::onMouseMove(const Event& e)
{
    const auto& event = static_cast<const MouseMoveEvent&>(e);

    float xOffset = event.xPos() - event.lastX();
    float yOffset = event.lastY() - event.yPos();

    processMouseMovement(xOffset, yOffset);
}

void Camera::processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch)
{
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (constrainPitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::onMouseScroll(const Event& e)
{
    const auto& event = static_cast<const MouseScrollEvent&>(e);
    zoom -= (float)event.yOffset();
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}
