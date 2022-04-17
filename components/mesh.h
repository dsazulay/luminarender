//
// Created by Diego Azulay on 14/04/22.
//

#ifndef INTERACTIVEGRAPHICS_MESH_H
#define INTERACTIVEGRAPHICS_MESH_H


#include <glm/glm.hpp>
#include <vector>

#include "../material.h"
#include "icomponent.h"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Mesh : public IComponent
{
public:
    Mesh();
    ~Mesh();
    void setPrimitives(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    void setPrimitives(std::pair<std::vector<Vertex>, std::vector<unsigned int>> primitives);

    unsigned int vao() const;
    int indicesSize() const;

    Material* material;

private:
    unsigned int VBO, EBO, VAO;

    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;

    void initMesh();
};


#endif //INTERACTIVEGRAPHICS_MESH_H
