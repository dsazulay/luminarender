#pragma once

#include "model_types.h"

#include <vector>
#include <string>

class Mesh
{
public:
    Mesh() = default;
    Mesh(MeshType meshType);
    Mesh(VertexIndexTuple m);

    int indicesCount() const;
    unsigned int vao() const;
    MeshType meshType() const;
    void meshType(MeshType m);
    std::string modelMat() const;

    void initMesh();

    MeshType m_meshType;
    unsigned int m_vbo, m_ebo, m_vao;
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::string m_modelMat;
};
