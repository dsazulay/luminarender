#pragma once

#include <glm/glm.hpp>
#include "../pch.h"
#include "material.h"
#include "../components/icomponent.h"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Mesh : public IComponent
{
public:
    Mesh(std::pair<std::vector<Vertex>, std::vector<unsigned int>> primitives);
    ~Mesh();
    void setPrimitives(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    void setPrimitives(std::pair<std::vector<Vertex>, std::vector<unsigned int>> primitives);

    int indicesSize() const;


    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;

};
