#include "mesh.h"

Mesh::Mesh(std::pair<std::vector<Vertex>, std::vector<unsigned int>> primitives)
{
    setPrimitives(primitives);
}

Mesh::~Mesh()
{

}

void Mesh::setPrimitives(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
{
    this->m_Vertices = vertices;
    this->m_Indices = indices;
}

void Mesh::setPrimitives(std::pair<std::vector<Vertex>, std::vector<unsigned int>> primitives)
{
    setPrimitives(primitives.first, primitives.second);
}

int Mesh::indicesSize() const
{
    return (int)m_Indices.size();
}
