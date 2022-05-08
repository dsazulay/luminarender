#pragma once

#include "irenderable.h"
#include "../assets/material.h"
#include <glm/glm.hpp>

class ShadowPass : public IRenderable
{
public:
    void render(Scene& scene) override;
    void shadowMaterial(Material* mat);
    glm::mat4 lightSpaceMatrix;

private:
    void updateLightMatrices(Entity* mainLight);
    void renderEntity(Entity& entity);

    Material* m_shadowMat;
};