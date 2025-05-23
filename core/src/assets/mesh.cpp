#include "mesh.h"

#include "primitives.h"

#include <glad/gl.h>
#include <glm/trigonometric.hpp>

Mesh::Mesh(MeshType meshType)
{
    VertexIndexTuple m = getNativeMeshByType(meshType);
    m_meshType = meshType;
    m_vertices = m.vertices;
    m_indices = m.indices;

    initMesh();
}

Mesh::Mesh(VertexIndexTuple m)
{
    m_vertices = m.vertices;
    m_indices = m.indices;
    m_modelMat = m.material;

    initMesh();
}

int Mesh::indicesCount() const
{
    return (int)m_indices.size();
}

unsigned int Mesh::vao() const
{
    return m_vao;
}

MeshType Mesh::meshType() const
{
    return m_meshType;
}

void Mesh::meshType(MeshType mt)
{
    m_meshType = mt;
}

std::string Mesh::modelMat() const
{
    return m_modelMat;
}

void Mesh::initMesh()
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));

    glBindVertexArray(0);
}
