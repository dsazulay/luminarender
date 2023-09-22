#pragma once

#include "../ecs.h"
#include "../assets/mesh.h"
#include "../assets/material.h"

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>

#include <vector>
#include <optional>

namespace ecs
{
struct Transform
{
    glm::vec3 position{0.0, 0.0, 0.0};
    glm::vec3 rotation{0.0, 0.0, 0.0};
    glm::vec3 scale{1.0, 1.0, 1.0};
    glm::quat quaternion{1.0, 0.0, 0.0, 0.0};
    glm::mat4 modelMatrix{1.0};

    std::optional<Entity> parent;
    std::vector<Entity> children;
};

enum class LightType
{
    DIRECTIONAL,
    POINT,
    SPOT
};

struct Light
{
    glm::vec3 color{0.8, 0.8, 0.7};
    glm::vec3 attenuation{1.0, 0.09, 0.032};
    float intensity{1.0};
    float cutoff{0.0};
    float outerCutoff{0.0};
    LightType lightType{LightType::DIRECTIONAL};
};

struct MeshRenderer
{
    Mesh* mesh;
    Material* material;
};
}

