#pragma once

#include "pch.h"
#include <glm/glm.hpp>

class Primitives
{
public:
    static std::pair<std::vector<Vertex>, std::vector<unsigned int>> getQuadPrimitives()
    {
        return getPrimitives(m_quadVertices, 32, m_quadIndices, 6);
    }

    static std::pair<std::vector<Vertex>, std::vector<unsigned int>> getCubePrimitives()
    {
        return getPrimitives(m_cubeVertices, 192, m_cubeIndices, 36);
    }

    static std::pair<std::vector<Vertex>, std::vector<unsigned int>> getCubeMapPrimitives()
    {
        return getMapPrimitives(cubeMapVertices, 72, m_cubeMapIndices, 36);
    }

    static std::pair<std::vector<Vertex>, std::vector<unsigned int>> getSpherePrimitives()
    {
        std::vector<Vertex> outVertices;
        std::vector<unsigned int> outIndices;

        const float PI = 3.14159265;
        const float radius = 0.5f;
        const int sectorCount = 36;
        const int stackCount = 18;

        float lengthInv = 1.0f / radius;
        float sectorStep = 2 * PI / sectorCount;
        float stackStep = PI / stackCount;
        float sectorAngle, stackAngle;
        float xy, z;

        for (int i = 0; i <= stackCount; i++)
        {
            stackAngle = PI / 2 - (float)i * stackStep;
            xy = radius * cos(stackAngle);
            z = radius * sin(stackAngle);

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

                outVertices.push_back(vert);
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
                    outIndices.push_back(k1);
                    outIndices.push_back(k2);
                    outIndices.push_back(k1 + 1);
                }

                if (i != (stackCount - 1))
                {
                    outIndices.push_back(k1 + 1);
                    outIndices.push_back(k2);
                    outIndices.push_back(k2 + 1);
                }
            }
        }

        return std::make_pair(outVertices, outIndices);
    }


private:
    static constexpr float m_quadVertices[32] = {
            // position           // normal            // uv
            -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
            1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
            1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
            -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
    };

    static constexpr unsigned int m_quadIndices[6] = {
            0, 1, 2, 2, 3, 0
    };

    static constexpr float m_cubeVertices[192] = {
            // position           // normal            // uv
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    };

    static constexpr unsigned int m_cubeIndices[36] = {
            0, 2, 1, 2, 0, 3,
            4, 5, 6, 6, 7, 4,
            8, 9, 10, 10, 11, 8,
            12, 14, 13, 14, 12, 15,
            16, 17, 18, 18, 19, 16,
            20, 22, 21, 22, 20, 23
    };

    static constexpr unsigned int m_cubeMapIndices[36] = {
            0, 1, 2, 2, 3, 0,
            4, 6, 5, 6, 4, 7,
            8, 10, 9, 10, 8, 11,
            12, 13, 14, 14, 15, 12,
            16, 18, 17, 18, 16, 19,
            20, 21, 22, 22, 23, 20
    };

    static constexpr float cubeMapVertices[72] = {
            // positions
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,

            -0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,

            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,

            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,

            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,

            -0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
    };

    static std::pair<std::vector<Vertex>, std::vector<unsigned int>> getPrimitives(float const* vertices, int verticesSize, unsigned int const* indices, int indicesSize)
    {
        std::vector<Vertex> outVertices;
        std::vector<unsigned int> outIndices;

        outVertices.reserve(verticesSize);
        for (int i = 0; i < verticesSize; i += 8)
        {
            Vertex vert{};
            glm::vec3 vec;

            vec.x = vertices[i];
            vec.y = vertices[i+1];
            vec.z = vertices[i+2];
            vert.position = vec;

            vec.x = vertices[i+3];
            vec.y = vertices[i+4];
            vec.z = vertices[i+5];
            vert.normal = vec;

            glm::vec2 uv;
            uv.x = vertices[i+6];
            uv.y = vertices[i+7];
            vert.texCoords = uv;

            outVertices.push_back(vert);
        }

        outIndices.reserve(indicesSize);
        for (int i = 0; i < indicesSize; i++)
        {
            outIndices.push_back(indices[i]);
        }

        return std::make_pair(outVertices, outIndices);
    }

    static std::pair<std::vector<Vertex>, std::vector<unsigned int>> getMapPrimitives(float const* vertices, int verticesSize, unsigned int const* indices, int indicesSize)
    {
        std::vector<Vertex> outVertices;
        std::vector<unsigned int> outIndices;

        outVertices.reserve(verticesSize);
        for (int i = 0; i < verticesSize; i += 3)
        {
            Vertex vert{};
            glm::vec3 vec;

            vec.x = vertices[i];
            vec.y = vertices[i+1];
            vec.z = vertices[i+2];
            vert.position = vec;

            outVertices.push_back(vert);
        }

        outIndices.reserve(indicesSize);
        for (int i = 0; i < indicesSize; i++)
        {
            outIndices.push_back(indices[i]);
        }

        return std::make_pair(outVertices, outIndices);
    }
};
