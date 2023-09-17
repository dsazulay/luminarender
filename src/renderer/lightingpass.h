#pragma once

#include "irenderable.h"
#include "../assets/material.h"
#include "../assets/mesh.h"
#include "../camera.h"

class LightingPass
{
public:
    LightingPass();
    void render();
    void camera(Camera* camera);

    unsigned int gposition;
    unsigned int gnormal;
    unsigned int galbedospec;

private:
    Mesh* m_fullscreenQuad;
    Camera* m_camera{};
    Material* m_material;
};
