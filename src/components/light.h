#pragma once

#include <glm/glm.hpp>
#include "icomponent.h"

enum LightType
{
    DIRECTIONAL,
    POINT,
    SPOT
};

struct LightUniformStruct
{
    glm::vec4 posAndCutoff;
    glm::vec4 dirAndOuterCutoff;
    glm::vec4 colorAndIntensity;
    glm::vec4 attenuationAndType;
};

struct Light : public IComponent
{
    LightType lightType;
    glm::vec3 color;
    float intensity;
    glm::vec3 attenuation;
    float cutoff;
    float outerCutoff;
    LightUniformStruct uniformStruct;
};
