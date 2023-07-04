#pragma once

#include "icomponent.h"
#include "../assets/mesh.h"

class Material;

struct MeshRenderer : public IComponent
{
    int indicesCount() const;
    unsigned int vao() const;
    MeshType meshType() const;

    Material* material;
    Mesh* mesh;
};
