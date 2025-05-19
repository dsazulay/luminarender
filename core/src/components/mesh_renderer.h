#pragma once

#include "../assets/mesh.h"

class Material;

struct MeshRenderer
{
    int indicesCount() const;
    unsigned int vao() const;
    MeshType meshType() const;

    Material* material;
    Mesh* mesh;
};
