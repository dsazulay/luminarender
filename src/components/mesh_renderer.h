#pragma once

#include "../assets/material.h"
#include "icomponent.h"
#include "../assets/mesh.h"

class MeshRenderer : public IComponent
{
public:
    unsigned int vao() const;
    void initMesh();

    Material* material;
    Mesh* mesh;

private:
    unsigned int VBO, EBO, VAO;

};
