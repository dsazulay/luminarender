//
// Created by Diego Azulay on 14/04/22.
//

#include "mesh.h"

#include "glad/glad.h"

Mesh::Mesh()
{

}

Mesh::~Mesh()
{

}

void Mesh::setPrimitives(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
{
    this->m_Vertices = vertices;
    this->m_Indices = indices;

    initMesh();
}

void Mesh::setPrimitives(std::pair<std::vector<Vertex>, std::vector<unsigned int>> primitives)
{
    setPrimitives(primitives.first, primitives.second);
}

unsigned int Mesh::vao() const
{
    return VAO;
}

int Mesh::indicesSize() const
{
    return (int)m_Indices.size();
}

void Mesh::initMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), &m_Vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    glBindVertexArray(0);
}