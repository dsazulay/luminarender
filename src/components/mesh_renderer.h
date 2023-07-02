#pragma once

#include "icomponent.h"
#include "../assets/material.h"
#include "../assets/mesh.h"

struct MeshRenderer : public IComponent
{
    int indicesCount() const;
    unsigned int vao() const;

    Material* material;
    Mesh* mesh;
};
