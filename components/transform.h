//
// Created by Diego Azulay on 14/04/22.
//

#ifndef INTERACTIVEGRAPHICS_TRANSFORM_H
#define INTERACTIVEGRAPHICS_TRANSFORM_H


#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "icomponent.h"

class Transform : public IComponent
{
public:
    Transform();
    ~Transform();

    glm::vec3 getDirection() const;

    void updateModelMatrix();
    void updateFromModelMatrix();

    glm::mat4 modelMatrix() const;
    void modelMatrix(glm::mat4 m);

    glm::vec3 position() const;
    void position(glm::vec3 p);

    glm::quat rotation();
    void rotation(glm::quat q);

    glm::vec3 eulerAngles();
    void eulerAngles(glm::vec3 r);

    glm::vec3 scale();
    void scale(glm::vec3 s);

private:
    glm::mat4 m_ModelMatrix;
    glm::vec3 m_Position;
    glm::quat m_Rotation;
    glm::vec3 m_EulerAngles;
    glm::vec3 m_Scale;
};


#endif //INTERACTIVEGRAPHICS_TRANSFORM_H
