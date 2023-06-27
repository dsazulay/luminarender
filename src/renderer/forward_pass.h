#pragma once

#include "irenderable.h"
#include "render_target.h"
#include <glm/glm.hpp>

class ForwardPass : public IRenderable
{
public:
    void render(Scene& scene) override;

    glm::mat4 lightSpaceMatrix;

    unsigned int irradianceMap;
    unsigned int prefilterMap;
    unsigned int brdfLUT;
    unsigned int shadowMap;


private:
    void renderEntity(Entity& entity, Entity* mainLight);
    void renderSkybox(Entity& skybox);
};
