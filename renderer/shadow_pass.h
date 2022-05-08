#pragma once

#include "render_target.h"
#include "irenderable.h"
#include "../assets/material.h"
#include <glm/glm.hpp>

class ShadowPass : public IRenderable
{
public:
    void render(std::list<Entity>& objects) override;
    void shadowMaterial(Material* mat);
    void mainLight(Entity* entity);
    glm::mat4 lightSpaceMatrix;

private:
    void updateLightMatrices();
    void renderEntity(Entity& entity);

    Entity* m_mainLight;
    Material* m_shadowMat;
};