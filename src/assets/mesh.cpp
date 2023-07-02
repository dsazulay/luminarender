#include "mesh.h"
#include "glm/trigonometric.hpp"

namespace primitives
{
    VertexIndexTuple quad()
    {
        VertexIndexTuple m;
        m.vertices = {
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        };

        m.indices = {0, 1, 2, 2, 3, 0};

        return m;
    }

    VertexIndexTuple cube()
    {
        VertexIndexTuple m;
         m.vertices = {
            {{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},
            {{-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},

            {{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
            {{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},

            {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
            {{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
            {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
            {{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},

            {{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
            {{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
            {{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},

            {{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},
            {{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
            {{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}},
            {{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},

            {{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},
            {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
            {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
            {{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
        };

        m.indices = {
            0,  2,  1,  2,  0,  3,
            4,  5,  6,  6,  7,  4,
            8,  9,  10, 10, 11, 8,
            12, 14, 13, 14, 12, 15,
            16, 17, 18, 18, 19, 16,
            20, 22, 21, 22, 20, 23
        };

        return m;
    }

    VertexIndexTuple sphere()
    {
        VertexIndexTuple m;

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
                vert.texCoords = uv;

                m.vertices.push_back(vert);
            }

        }

        unsigned int k1, k2;
        for (int i = 0; i < stackCount; i++)
        {
            k1 = i * (sectorCount + 1);
            k2 = k1 + sectorCount + 1;

            for (int j = 0; j < sectorCount; j++, k1++, k2++)
            {
                if (i != 0)
                {
                    m.indices.push_back(k1);
                    m.indices.push_back(k2);
                    m.indices.push_back(k1 + 1);
                }

                if (i != (stackCount - 1))
                {
                    m.indices.push_back(k1 + 1);
                    m.indices.push_back(k2);
                    m.indices.push_back(k2 + 1);
                }
            }
        }

        return m;
    }

    VertexIndexTuple cubeMap()
    {
        VertexIndexTuple m;
        m.vertices = {
            {{-0.5f, -0.5f, -0.5f}},
            {{ 0.5f, -0.5f, -0.5f}},
            {{ 0.5f,  0.5f, -0.5f}},
            {{-0.5f,  0.5f, -0.5f}},

            {{-0.5f, -0.5f,  0.5f}},
            {{ 0.5f, -0.5f,  0.5f}},
            {{ 0.5f,  0.5f,  0.5f}},
            {{-0.5f,  0.5f,  0.5f}},

            {{-0.5f,  0.5f,  0.5f}},
            {{-0.5f,  0.5f, -0.5f}},
            {{-0.5f, -0.5f, -0.5f}},
            {{-0.5f, -0.5f,  0.5f}},

            {{ 0.5f,  0.5f,  0.5f}},
            {{ 0.5f,  0.5f, -0.5f}},
            {{ 0.5f, -0.5f, -0.5f}},
            {{ 0.5f, -0.5f,  0.5f}},

            {{-0.5f, -0.5f, -0.5f}},
            {{ 0.5f, -0.5f, -0.5f}},
            {{ 0.5f, -0.5f,  0.5f}},
            {{-0.5f, -0.5f,  0.5f}},

            {{-0.5f,  0.5f, -0.5f}},
            {{ 0.5f,  0.5f, -0.5f}},
            {{ 0.5f,  0.5f,  0.5f}},
            {{-0.5f,  0.5f,  0.5f}},

        };

        m.indices = {
            0,  1,  2,  2,  3,  0,
            4,  6,  5,  6,  4,  7,
            8,  10, 9,  10, 8,  11,
            12, 13, 14, 14, 15, 12,
            16, 18, 17, 18, 16, 19,
            20, 21, 22, 22, 23, 20
        };

        return m;
    }

    VertexIndexTuple triangleMap()
    {
        VertexIndexTuple m;
        m.vertices = {
            {{-1.0f, -1.0f, 0.999f}},
            {{ 3.0f, -1.0f, 0.999f}},
            {{-1.0f,  3.0f, 0.999f}},
        };

        m.indices = {
            0, 1, 2
        };

        return m;
    }
}

Mesh::Mesh(BasicMeshType meshType)
{
    VertexIndexTuple m;
    switch (meshType)
    {
        case BasicMeshType::Quad:
            m = primitives::quad();
            break;
        case BasicMeshType::Cube:
            m = primitives::cube();
            break;
        case BasicMeshType::Sphere:
            m = primitives::sphere();
            break;
        case BasicMeshType::CubeMap:
            m = primitives::cubeMap();
            break;
        case BasicMeshType::TriangleMap:
            m = primitives::triangleMap();
            break;
    }

    m_vertices = m.vertices;
    m_indices = m.indices;
}

Mesh::Mesh(VertexIndexTuple m)
{
    m_vertices = m.vertices;
    m_indices = m.indices;
}

int Mesh::indicesSize() const
{
    return (int)m_indices.size();
}
