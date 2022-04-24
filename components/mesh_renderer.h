//
// Created by Diego Azulay on 19/04/22.
//

#ifndef INTERACTIVEGRAPHICS_MESH_RENDERER_H
#define INTERACTIVEGRAPHICS_MESH_RENDERER_H


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


#endif //INTERACTIVEGRAPHICS_MESH_RENDERER_H
