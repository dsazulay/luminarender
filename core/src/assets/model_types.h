#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <cstdint>
#include <vector>
#include <string>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
};

struct VertexIndexTuple
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    // TODO: remove this
    std::string material;
};

enum class MeshType : uint8_t
{
    None = 0,
    Quad,
    Cube,
    Sphere,
    Custom,
    CubeMap,
    TriangleMap,
};

