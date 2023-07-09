#pragma once

#include "irenderable.h"

class Camera;
class Material;

class NormalVisualizerPass : IRenderable
{
public:
    NormalVisualizerPass();
    void render(Scene& scene) override;
    void camera(Camera* camera);

private:
    void renderNormalVectorOfEntity(Entity &entity);

    Camera* m_camera{};
    Material* m_material;
};
