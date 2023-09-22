#include "light_system.h"

#include "../components/components.h"
#include "../log.h"

void LightSystem::update(UniformBufferObject& lightUBO, ecs::Coordinator& coordinator)
{
    std::size_t offset = sizeof(glm::vec4);
    std::size_t uniformStructSize = sizeof(LightUniformStruct);

    glm::vec4 nLights = glm::vec4(2, 1, 1, 0);
    lightUBO.setBufferData(0, sizeof(glm::vec4), &nLights);

    for (auto& entity : m_entities)
    {
        auto transform = coordinator.getComponent<ecs::Transform>(entity);
        auto light = coordinator.getComponent<ecs::Light>(entity);

        LightUniformStruct lightUniform;
        lightUniform.posAndCutoff = glm::vec4(transform.position, light.cutoff);
        lightUniform.dirAndOuterCutoff = glm::vec4(getDirection(transform.quaternion), light.outerCutoff);
        lightUniform.colorAndIntensity = glm::vec4(light.color, light.intensity);
        lightUniform.attenuationAndType = glm::vec4(light.attenuation, light.lightType);

        lightUBO.setBufferData(offset, uniformStructSize, &lightUniform);
        offset += uniformStructSize;
    }
}
glm::vec3 LightSystem::getDirection(glm::quat rotation)
{
    return rotation * glm::vec3(0.0, 0.0, -1.0);
}
