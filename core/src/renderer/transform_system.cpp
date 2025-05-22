#include "transform_system.h"

#include "../log.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

void TransformSystem::init(ecs::Coordinator* coordinator)
{
    m_coordinator = coordinator;
}

void TransformSystem::update()
{
    for (auto entity : m_entities)
    {
        updateModelMatrix(entity);
    }
}

void TransformSystem::updateHierarchically()
{
    updateRootEntities();
    for (auto entity : m_rootEntities)
    {
        updateSeflAndChilren(entity);
    }
}

void TransformSystem::updateModelMatrix(ecs::Entity entity)
{
    auto& transform = m_coordinator->getComponent<ecs::Transform>(entity);
    updateModelMatrix(transform);
}

void TransformSystem::updateFromModelMatrix(ecs::Entity entity)
{
    auto& transform = m_coordinator->getComponent<ecs::Transform>(entity);
}

void TransformSystem::updateSeflAndChilren(ecs::Entity entity) 
{
    auto& transform = m_coordinator->getComponent<ecs::Transform>(entity);
    updateModelMatrix(transform);
    if (auto parent = transform.parent)
    {
        auto& parentTransform = m_coordinator->getComponent<ecs::Transform>(*parent);
        transform.modelMatrix = parentTransform.modelMatrix * transform.modelMatrix;
    }

    for (auto child : transform.children)
    {
        updateSeflAndChilren(child);
    }
}

void TransformSystem::updateRootEntities()
{
    m_rootEntities.clear();

    for (auto entity : m_entities)
    {
        auto& transform = m_coordinator->getComponent<ecs::Transform>(entity);
        if (auto parent = transform.parent)
            continue;

        m_rootEntities.push_back(entity);
    }
}

void TransformSystem::addChild(ecs::Entity parent, ecs::Entity child)
{
    auto& transform = m_coordinator->getComponent<ecs::Transform>(child);
    auto& parentTransform = m_coordinator->getComponent<ecs::Transform>(parent);
    transform.parent = parent;
    parentTransform.children.push_back(child);

    transform.modelMatrix = glm::inverse(parentTransform.modelMatrix) * transform.modelMatrix;
    updateFromModelMatrix(transform);
    transform.rotation -= parentTransform.rotation;
}

void TransformSystem::updateModelMatrix(ecs::Transform& transform)
{
    transform.quaternion = glm::quat(glm::radians(transform.rotation));
    transform.modelMatrix = glm::mat4(1.0);
    transform.modelMatrix = glm::translate(transform.modelMatrix, transform.position);
    transform.modelMatrix = transform.modelMatrix * glm::mat4_cast(transform.quaternion);
    transform.modelMatrix = glm::scale(transform.modelMatrix, transform.scale);
}

void TransformSystem::updateFromModelMatrix(ecs::Transform& transform)
{
    glm::vec3 skew;
    glm::vec4 persp;
    glm::decompose(transform.modelMatrix, transform.scale, transform.quaternion, transform.position, skew, persp);
    transform.rotation = glm::eulerAngles(transform.quaternion);
}

