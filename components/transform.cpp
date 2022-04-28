//
// Created by Diego Azulay on 14/04/22.
//

#include <iostream>
#include "transform.h"
#include <glm/gtx/matrix_decompose.hpp>

Transform::Transform()
{
    m_ModelMatrix = glm::mat4(1.0f);
    m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
    m_Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    m_EulerAngles = glm::vec3(0.0f, 0.0f, 0.0f);
    m_Scale = glm::vec3(1.0f, 1.0f, 1.0f);
}

Transform::~Transform()
{

}

glm::vec3 Transform::getDirection() const
{
    glm::vec3 forward(0.0f, 0.0f, -1.0f);

    return m_Rotation * forward;
}

void Transform::updateModelMatrix()
{
    m_Rotation = glm::quat(glm::radians(m_EulerAngles));
    m_ModelMatrix = glm::mat4(1.0f);
    m_ModelMatrix = glm::translate(m_ModelMatrix, m_Position);
    m_ModelMatrix = m_ModelMatrix * glm::mat4_cast(m_Rotation);
    m_ModelMatrix = glm::scale(m_ModelMatrix, m_Scale);
}

glm::mat4 Transform::modelMatrix() const
{
    return m_ModelMatrix;
}

void Transform::modelMatrix(glm::mat4 m)
{
    m_ModelMatrix = m;
}

glm::vec3 Transform::position() const
{
    return m_Position;
}

void Transform::position(glm::vec3 p)
{
    m_Position = p;
}

glm::quat Transform::rotation()
{
    return m_Rotation;
}

void Transform::rotation(glm::quat q)
{
    m_Rotation = q;
}

glm::vec3 Transform::eulerAngles()
{
    return m_EulerAngles;
}

void Transform::eulerAngles(glm::vec3 r)
{
    m_EulerAngles = r;
}

glm::vec3 Transform::scale()
{
    return m_Scale;
}

void Transform::scale(glm::vec3 s)
{
    m_Scale = s;
}

void Transform::updateFromModelMatrix()
{
    glm::vec3 skew;
    glm::vec4 persp;
    glm::decompose(m_ModelMatrix, m_Scale, m_Rotation, m_Position, skew, persp);
//    m_Position = m_ModelMatrix[3];
//    m_EulerAngles = glm::eulerAngles(glm::quat(m_ModelMatrix));
//    for (int i = 0; i < 3; i++)
//    {
//        m_Scale[i] = glm::length(m_ModelMatrix[i]);
////        mat[i]
//    }
    eulerAngles(glm::eulerAngles(m_Rotation));
}


