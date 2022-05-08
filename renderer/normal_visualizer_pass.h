#pragma once

#include "irenderable.h"
#include "../assets/material.h"
#include "../camera.h"

class NormalVisualizerPass : IRenderable
{
public:
    void render(Scene& scene) override;
    void camera(Camera* camera);
    Material* m_material{};

private:
    void renderNormalVectorOfEntity(Entity &entity);

    Camera* m_camera{};
};