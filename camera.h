//
// Created by Diego Azulay on 13/04/22.
//

#ifndef INTERACTIVEGRAPHICS_CAMERA_H
#define INTERACTIVEGRAPHICS_CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "events/event.h"

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =   0.0f;
const float SPEED       =   2.5f;
const float SENSITIVITY =   0.1f;
const float ZOOM        =  45.0f;

class Camera
{
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;

    float movementSpeed;
    float mouseSensitivity;
    float zoom;


    Camera(glm::vec3 position);
    glm::mat4 getViewMatrix();
    void processKeyboard(Camera_Movement direction, float deltaTime);

private:
    void onMouseScroll(const Event& e);
    void onMouseMove(const Event& e);

    void processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true);
    void updateCameraVectors();
};


#endif //INTERACTIVEGRAPHICS_CAMERA_H
