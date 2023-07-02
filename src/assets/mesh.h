#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <vector>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct VertexIndexTuple
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

enum class BasicMeshType
{
    Quad,
    Cube,
    Sphere,
    CubeMap,
    TriangleMap,
};

namespace primitives
{
    VertexIndexTuple quad();
    VertexIndexTuple cube();
    VertexIndexTuple sphere();
    VertexIndexTuple cubeMap();
    VertexIndexTuple triangleMap();
}

class Mesh
{
public:
    Mesh(BasicMeshType meshType);
    Mesh(VertexIndexTuple m);

    int indicesSize() const;

    unsigned int vao() const;

    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;

private:
    void initMesh();

    unsigned int m_vbo, m_ebo, m_vao;
};
