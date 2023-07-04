#include "mesh_renderer.h"

#include "../assets/material.h"


int MeshRenderer::indicesCount() const
{
    return mesh->indicesCount();
}

unsigned int MeshRenderer::vao() const
{
    return mesh->vao();
}

MeshType MeshRenderer::meshType() const
{
    if (mesh == nullptr)
        return MeshType::None;
    return mesh->meshType();
}
