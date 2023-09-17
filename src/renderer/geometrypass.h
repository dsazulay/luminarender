#pragma once

#include "irenderable.h"
#include "../assets/material.h"
#include "../camera.h"

class GeometryPass
{
public:
    GeometryPass();
    void render(Scene& scene);
    void camera(Camera* camera);

private:
    void renderEntity(Entity& entity);

    Camera* m_camera;
    Material* m_material;
};
