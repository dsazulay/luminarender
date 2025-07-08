#include "primitives.h"

#include <glm/trigonometric.hpp>

VertexIndexTuple getQuad()
{
    std::vector<Vertex> vertices{
        {
            .position = { -0.5, -0.5, 0.0 },
            .normal = { 0.0, 0.0, 1.0 },
            .texcoord = { 0.0, 0.0 },
            .tangent = { 1.0, 0.0, 0.0 },
        },
        {
            .position = { 0.5, -0.5, 0.0 },
            .normal = { 0.0, 0.0, 1.0 },
            .texcoord = { 1.0, 0.0 },
            .tangent = { 1.0, 0.0, 0.0 },
        },
        {
            .position = { -0.5, 0.5, 0.0 },
            .normal = { 0.0, 0.0, 1.0 },
            .texcoord = { 0.0, 1.0 },
            .tangent = { 1.0, 0.0, 0.0 },
        },
        {
            .position = { 0.5, 0.5, 0.0 },
            .normal = { 0.0, 0.0, 1.0 },
            .texcoord = { 1.0, 1.0 },
            .tangent = { 1.0, 0.0, 0.0 },
        }
    };

    std::vector<unsigned int> indices{ 0, 1, 2, 3, 2, 1 };

    return { .vertices = vertices, .indices = indices };
}

VertexIndexTuple getCube()
{
    std::vector<Vertex> vertices{
        {
            .position = {-0.5f, -0.5f, -0.5f},
            .normal = { 0.0f,  0.0f, -1.0f},
            .texcoord = {0.0f, 0.0f},
            .tangent = { -1.0, 0.0, 0.0 },
        },
        {
            .position = { 0.5f, -0.5f, -0.5f},
            .normal = { 0.0f,  0.0f, -1.0f},
            .texcoord = {1.0f, 0.0f},
            .tangent = { -1.0, 0.0, 0.0 },
        },
        {
            .position = { 0.5f,  0.5f, -0.5f},
            .normal = { 0.0f,  0.0f, -1.0f},
            .texcoord = {1.0f, 1.0f},
            .tangent = { -1.0, 0.0, 0.0 },
        },
        {
            .position = {-0.5f,  0.5f, -0.5f},
            .normal = { 0.0f,  0.0f, -1.0f},
            .texcoord = {0.0f, 1.0f},
            .tangent = { -1.0, 0.0, 0.0 },
        },

        {
            .position = {-0.5f, -0.5f,  0.5f},
            .normal = { 0.0f,  0.0f,  1.0f},
            .texcoord = {0.0f, 0.0f},
            .tangent = { 1.0, 0.0, 0.0 },
        },
        {
            .position = { 0.5f, -0.5f,  0.5f},
            .normal = { 0.0f,  0.0f,  1.0f},
            .texcoord = {1.0f, 0.0f},
            .tangent = { 1.0, 0.0, 0.0 },
        },
        {
            .position = { 0.5f,  0.5f,  0.5f},
            .normal = { 0.0f,  0.0f,  1.0f},
            .texcoord = {1.0f, 1.0f},
            .tangent = { 1.0, 0.0, 0.0 },
        },
        {
            .position = {-0.5f,  0.5f,  0.5f},
            .normal = { 0.0f,  0.0f,  1.0f},
            .texcoord = {0.0f, 1.0f},
            .tangent = { 1.0, 0.0, 0.0 },
        },

        {
            .position = {-0.5f,  0.5f,  0.5f},
            .normal = {-1.0f,  0.0f,  0.0f},
            .texcoord = {1.0f, 0.0f},
            .tangent = { 0.0, 0.0, 1.0 },
        },
        {
            .position = {-0.5f,  0.5f, -0.5f},
            .normal = {-1.0f,  0.0f,  0.0f},
            .texcoord = {1.0f, 1.0f},
            .tangent = { 0.0, 0.0, 1.0 },
        },
        {
            .position = {-0.5f, -0.5f, -0.5f},
            .normal = {-1.0f,  0.0f,  0.0f},
            .texcoord = {0.0f, 1.0f},
            .tangent = { 0.0, 0.0, 1.0 },
        },
        {
            .position = {-0.5f, -0.5f,  0.5f},
            .normal = {-1.0f,  0.0f,  0.0f},
            .texcoord = {0.0f, 0.0f},
            .tangent = { 0.0, 0.0, 1.0 },
        },

        {
            .position = { 0.5f,  0.5f,  0.5f},
            .normal = { 1.0f,  0.0f,  0.0f},
            .texcoord = {1.0f, 0.0f},
            .tangent = { 0.0, 0.0, -1.0 },
        },
        {
            .position = { 0.5f,  0.5f, -0.5f},
            .normal = { 1.0f,  0.0f,  0.0f},
            .texcoord = {1.0f, 1.0f},
            .tangent = { 0.0, 0.0, -1.0 },
        },
        {
            .position = { 0.5f, -0.5f, -0.5f},
            .normal = { 1.0f,  0.0f,  0.0f},
            .texcoord = {0.0f, 1.0f},
            .tangent = { 0.0, 0.0, -1.0 },
        },
        {
            .position = { 0.5f, -0.5f,  0.5f},
            .normal = { 1.0f,  0.0f,  0.0f},
            .texcoord = {0.0f, 0.0f},
            .tangent = { 0.0, 0.0, -1.0 },
        },

        {
            .position = {-0.5f, -0.5f, -0.5f},
            .normal = { 0.0f, -1.0f,  0.0f},
            .texcoord = {0.0f, 1.0f},
            .tangent = { 1.0, 0.0, 0.0 },
        },
        {
            .position = { 0.5f, -0.5f, -0.5f},
            .normal = { 0.0f, -1.0f,  0.0f},
            .texcoord = {1.0f, 1.0f},
            .tangent = { 1.0, 0.0, 0.0 },
        },
        {
            .position = { 0.5f, -0.5f,  0.5f},
            .normal = { 0.0f, -1.0f,  0.0f},
            .texcoord = {1.0f, 0.0f},
            .tangent = { 1.0, 0.0, 0.0 },
        },
        {
            .position = {-0.5f, -0.5f,  0.5f},
            .normal = { 0.0f, -1.0f,  0.0f},
            .texcoord = {0.0f, 0.0f},
            .tangent = { 1.0, 0.0, 0.0 },
        },

        {
            .position = {-0.5f,  0.5f, -0.5f},
            .normal = { 0.0f,  1.0f,  0.0f},
            .texcoord = {0.0f, 1.0f},
            .tangent = { 1.0, 0.0, 0.0 },
        },
        {
            .position = { 0.5f,  0.5f, -0.5f},
            .normal = { 0.0f,  1.0f,  0.0f},
            .texcoord = {1.0f, 1.0f},
            .tangent = { 1.0, 0.0, 0.0 },
        },
        {
            .position = { 0.5f,  0.5f,  0.5f},
            .normal = { 0.0f,  1.0f,  0.0f},
            .texcoord = {1.0f, 0.0f},
            .tangent = { 1.0, 0.0, 0.0 },
        },
        {
            .position = {-0.5f,  0.5f,  0.5f},
            .normal = { 0.0f,  1.0f,  0.0f},
            .texcoord = {0.0f, 0.0f},
            .tangent = { 1.0, 0.0, 0.0 },
        },
    };

    std::vector<unsigned int> indices = {
        0,  2,  1,  2,  0,  3,
        4,  5,  6,  6,  7,  4,
        8,  9,  10, 10, 11, 8,
        12, 14, 13, 14, 12, 15,
        16, 17, 18, 18, 19, 16,
        20, 22, 21, 22, 20, 23
    };

    return { .vertices = vertices, .indices = indices };
}

VertexIndexTuple getSphere()
{
    std::vector<Vertex> vertices;

    const float PI = 3.14159265;
    const float radius = 0.5f;
    const int sectorCount = 36;
    const int stackCount = 18;

    const float lengthInv = 1.0f / radius;
    const float sectorStep = 2 * PI / sectorCount;
    const float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;
    float xy, z;

    for (int i = 0; i <= stackCount; i++)
    {
        stackAngle = PI / 2 - (float)i * stackStep;
        xy = radius * glm::cos(stackAngle);
        z = radius * glm::sin(stackAngle);

        for (int j = 0; j <= sectorCount; j++)
        {
            Vertex vert{};
            glm::vec3 vec;

            sectorAngle = (float)j * sectorStep;
            vec.x = xy * cos(sectorAngle);
            vec.y = xy * sin(sectorAngle);
            vec.z = z;
            vert.position = vec;

            vec.x = vec.x * lengthInv;
            vec.y = vec.y * lengthInv;
            vec.z = vec.z * lengthInv;
            vert.normal = vec;

            glm::vec2 uv;
            uv.x = (float)j / sectorCount;
            uv.y = (float)i / stackCount;
            vert.texcoord = uv;

            vertices.push_back(vert);
        }

    }

    std::vector<unsigned int> indices;

    unsigned int k1, k2;
    for (int i = 0; i < stackCount; i++)
    {
        k1 = i * (sectorCount + 1);
        k2 = k1 + sectorCount + 1;

        for (int j = 0; j < sectorCount; j++, k1++, k2++)
        {
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stackCount - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    return { .vertices = vertices, .indices = indices };
}

VertexIndexTuple getCubeMap()
{
    std::vector<Vertex> vertices = {
        { .position = {-0.5f, -0.5f, -0.5f} },
        { .position = { 0.5f, -0.5f, -0.5f} },
        { .position = { 0.5f,  0.5f, -0.5f} },
        { .position = {-0.5f,  0.5f, -0.5f} },

        { .position = {-0.5f, -0.5f,  0.5f} },
        { .position = { 0.5f, -0.5f,  0.5f} },
        { .position = { 0.5f,  0.5f,  0.5f} },
        { .position = {-0.5f,  0.5f,  0.5f} },

        { .position = {-0.5f,  0.5f,  0.5f} },
        { .position = {-0.5f,  0.5f, -0.5f} },
        { .position = {-0.5f, -0.5f, -0.5f} },
        { .position = {-0.5f, -0.5f,  0.5f} },

        { .position = { 0.5f,  0.5f,  0.5f} },
        { .position = { 0.5f,  0.5f, -0.5f} },
        { .position = { 0.5f, -0.5f, -0.5f} },
        { .position = { 0.5f, -0.5f,  0.5f} },

        { .position = {-0.5f, -0.5f, -0.5f} },
        { .position = { 0.5f, -0.5f, -0.5f} },
        { .position = { 0.5f, -0.5f,  0.5f} },
        { .position = {-0.5f, -0.5f,  0.5f} },

        { .position = {-0.5f,  0.5f, -0.5f} },
        { .position = { 0.5f,  0.5f, -0.5f} },
        { .position = { 0.5f,  0.5f,  0.5f} },
        { .position = {-0.5f,  0.5f,  0.5f} },

    };

    std::vector<unsigned int> indices = {
        0,  1,  2,  2,  3,  0,
        4,  6,  5,  6,  4,  7,
        8,  10, 9,  10, 8,  11,
        12, 13, 14, 14, 15, 12,
        16, 18, 17, 18, 16, 19,
        20, 21, 22, 22, 23, 20
    };

    return { .vertices = vertices, .indices = indices };
}

VertexIndexTuple getTriangleMap()
{
    std::vector<Vertex> vertices{
        { .position = {-1.0f, -1.0f, 0.999f} },
        { .position = { 3.0f, -1.0f, 0.999f} },
        { .position = {-1.0f,  3.0f, 0.999f} },
    };

    std::vector<unsigned int> indices{ 0, 1, 2 };

    return { .vertices = vertices, .indices = indices };
}

VertexIndexTuple getNativeMeshByType(MeshType type)
{
    switch (type) {
    case MeshType::Quad:
        return getQuad();
    case MeshType::Cube:
        return getCube();
    case MeshType::Sphere:
        return getSphere();
    case MeshType::CubeMap:
        return getCubeMap();
    case MeshType::TriangleMap:
        return getTriangleMap();
    default:
        return {};
    }
}
