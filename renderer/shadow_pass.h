#pragma once

#include "render_system.h"
#include "irenderable.h"
#include "../assets/material.h"
#include <glm/glm.hpp>

class ShadowPass : public RenderSystem, IRenderable
{
public:
    ShadowPass(int width, int height, FrameBuffer::Type type);
    void render(std::list<Entity>& objects) override;
    void init();
    void shadowMaterial(Material* mat);
    void mainLight(Entity* entity);
    glm::mat4 lightSpaceMatrix;

private:
    void clearFrameBuffer() override;
    void updateLightMatrices();
    void renderEntity(Entity& entity);

    Entity* m_mainLight;
    Material* m_shadowMat;
};