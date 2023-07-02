#include "mesh_renderer.h"

int MeshRenderer::indicesCount() const
{
    return mesh->indicesCount();
}

unsigned int MeshRenderer::vao() const
{
    return mesh-> vao();
}
