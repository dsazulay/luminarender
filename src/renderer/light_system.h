#pragma once

#include "../ecs.h"
#include "../components/components.h"
#include "../renderer/uniform_buffer_object.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>


struct LightUniformStruct
{
    glm::vec4 posAndCutoff;
    glm::vec4 dirAndOuterCutoff;
    glm::vec4 colorAndIntensity;
    glm::vec4 attenuationAndType;
};

class LightSystem : public ecs::System
{
public:
    void update(UniformBufferObject& lightUBO, ecs::Coordinator* coordinator);
private:
    glm::mat4 getLightMatrix(ecs::Transform& transform);
    glm::vec4 getShadowLightPos(ecs::Transform& transform);
    glm::vec3 getDirection(glm::quat rotation);
};
