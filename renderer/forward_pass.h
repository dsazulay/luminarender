#pragma once

#include "irenderable.h"
#include "render_system.h"

class ForwardPass : public RenderSystem, IRenderable
{
    void render(std::list<Entity>& objects) override;
};
