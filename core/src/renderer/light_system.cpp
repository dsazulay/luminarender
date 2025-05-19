#include "light_system.h"

#include "../components/components.h"
#include "../log.h"

void LightSystem::init(ecs::Coordinator* coordinator)
{
    m_coordinator = coordinator;
}

void LightSystem::update(UniformBufferObject& lightUBO)
{
    std::size_t offset = sizeof(glm::vec4);
    std::size_t uniformStructSize = sizeof(LightUniformStruct);

    glm::vec4 nLights = glm::vec4(1, 0, 0, 0);
    lightUBO.setBufferData(0, sizeof(glm::vec4), &nLights);
    // TODO: temporarily using the first light as main light for shadow casting
    auto& mainLightTransform = m_coordinator->getComponent<ecs::Transform>(
            *m_entities.begin());
    glm::vec4 shadowLightPos = getShadowLightPos(mainLightTransform);
    lightUBO.setBufferData(offset, sizeof(glm::vec4), &shadowLightPos);
    offset += sizeof(glm::vec4);
    glm::mat4 lightMatrix = getLightMatrix(mainLightTransform);
    lightUBO.setBufferData(offset, sizeof(glm::mat4), &lightMatrix);
    offset += sizeof(glm::mat4);

    for (auto entity : m_entities)
    {
        auto& transform = m_coordinator->getComponent<ecs::Transform>(entity);
        auto& light = m_coordinator->getComponent<ecs::Light>(entity);

        LightUniformStruct lightUniform{};
        lightUniform.posAndCutoff = glm::vec4(transform.position, light.cutoff);
        lightUniform.dirAndOuterCutoff = glm::vec4(getDirection(
                    transform.quaternion), light.outerCutoff);
        lightUniform.colorAndIntensity = glm::vec4(light.color, light.intensity);
        lightUniform.attenuationAndType = glm::vec4(light.attenuation, 
                light.lightType);

        lightUBO.setBufferData(offset, uniformStructSize, &lightUniform);
        offset += uniformStructSize;
    }
}

glm::mat4 LightSystem::getLightMatrix(ecs::Transform& transform)
{
    const float near_plane = 0.1f;
    const float far_plane = 40.0f;
    const float dimensions = 20.0f;
    glm::mat4 lightProjection = glm::ortho(-dimensions, dimensions, 
            -dimensions, dimensions, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(getDirection(transform.quaternion) * -10.f,
            getDirection(transform.quaternion), glm::vec3(0.0, 1.0, 0.0));
    return lightProjection * lightView;
}

glm::vec4 LightSystem::getShadowLightPos(ecs::Transform& transform)
{
    return glm::vec4(getDirection(transform.quaternion) * -10.0f, 1.0);
}

glm::vec3 LightSystem::getDirection(glm::quat rotation)
{
    return rotation * glm::vec3(0.0, 0.0, -1.0);
}
