#pragma once

#include "irenderable.h"
#include "render_system.h"
#include <glm/glm.hpp>

class ForwardPass : public RenderSystem, IRenderable
{
public:
    ForwardPass(int width, int height, FrameBuffer::Type type);

    void render(std::list<Entity>& objects) override;

    glm::mat4 lightSpaceMatrix;
    Entity* m_light;

    unsigned int irradianceMap;
    unsigned int prefilterMap;
    unsigned int brdfLUT;


private:
    void renderEntity(Entity& entity);
};
