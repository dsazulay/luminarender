#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <vector>
#include <string>

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
    std::string material;
};

enum class MeshType
{
    None,
    Quad,
    Cube,
    Sphere,
    Custom,
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
    Mesh(MeshType meshType);
    Mesh(VertexIndexTuple m);

    int indicesCount() const;
    unsigned int vao() const;
    MeshType meshType() const;
    void meshType(MeshType m);
    std::string modelMat() const;

private:
    void initMesh();

    MeshType m_meshType;
    unsigned int m_vbo, m_ebo, m_vao;
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::string m_modelMat;
};
