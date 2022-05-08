#pragma once

#include "irenderable.h"
#include "render_target.h"
#include <glm/glm.hpp>

class ForwardPass : public IRenderable
{
public:
    void render(std::list<Entity>& objects) override;

    glm::mat4 lightSpaceMatrix;
    Entity* m_light;

    unsigned int irradianceMap;
    unsigned int prefilterMap;
    unsigned int brdfLUT;
    unsigned int shadowMap;


private:
    void renderEntity(Entity& entity);
};
